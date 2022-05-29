#ifndef _DIBER_H_
#define _DIBER_H_
#include<iostream>
#include<memory>
#include<atomic>
#include<functional>
#include"thread.h"
#include<ucontext.h>
#include"Config.h"
#include"macro.h"
using namespace std;
static atomic<uint64_t>s_fiber_id(0);
static atomic<uint64_t>s_fiber_count(0);
class Fiber;

//拿到当前协程
static thread_local Fiber*t_fiber=NULL;
//相当于main协程
//static thread_local Fiber::ptr s_fiber=NULL;


//协程栈大小
static ConfigVar<uint32_t>::ptr g_stack_size=Config::LookUp<uint32_t>("fiber.stack.size",1024*1024,"fiber stack size");
class MallocStack{
  public:
    static void* Alloc(size_t size){
      return malloc(size);
    }
    static void Dealloc(void *vp,size_t size){
      free(vp);
    }
};
class Fiber:public enable_shared_from_this<Fiber>{
  public:
 typedef   shared_ptr<Fiber>ptr;
    enum State{
      INIT,
      HOLD,
      EXEC,
      TERM,
      READY,
      EXCEP
    };
  private:
    //将当前函数执行栈变成
    //将线程上下文变成一个协程的类
    //赋给协程的上下文
    Fiber();
  public:
    //创建协程
    Fiber(function<void()>cb,size_t stacksize=0);
    ~Fiber();
    //重置协程函数并重置状态
    //要么执行(INIT)，要么结束(TERM)
    void reset(function<void()>cd);
    //-将当前协程和正在执行的协程交换
    void SwapIn();
    //设置当前线程
    void SetThis(Fiber*f);
    //协程交出执行全，当前协程切换到后台  子协程------>主协程
    void swapOut();
    //返回当前执行的协程
    static Fiber::ptr GetThis();
    //让出执行权(协程切换到后台)并设置状态尾ready
    static void YieIdToReady();
    //让出执行权并设置状态尾Hold
    static void YieIdToHold();
    //协程数
    static  uint64_t TotalFibers();
    static void MainFunc();
    static uint32_t GetFiberId(){
      if(t_fiber){
        return t_fiber->id;
      }
      return 0;
      
    }
  private:
   uint64_t id=0;          //协程id
   uint32_t m_stacksize=0; //协程栈大小
   State m_state=INIT;     //协程状态


   ucontext_t m_ctx;
   void *m_stack=NULL;    //栈空间
   function<void()> cd;
/*
 *
 * 协程主要解决异步的事件，关注的并不是并发
 * 做的协程是每个线程中都有一个main协程，所有的协程切换都与main协程打交道
 *                               thread---->main_fibe
 *
 *                                               ^
 *                                               |
 *
 *                                               |
 *                                               v
 *                                           sub_fiber   创建协程时得回到main协程
 *                                                       子协程退出将执行控制权退
 *                                                       回到main协程，再去协调其
 *                                                       他的协程
 *    主协程创建子协程   子协程执行完回到主协程
 *
 * */
};



//相当于main协程
static thread_local Fiber::ptr s_fiber=NULL;
Fiber::Fiber(){
      m_state=EXEC;
      SetThis(this);
      //获取当前线程的上下文信息
      if(getcontext(&m_ctx)){
        //打印当前栈信息
        ASSERT2(false,"getcontext");
      }
      //协程数++
      ++s_fiber_count;
      cout<<"fibre:fiber id="<<id<<endl;;
    }
Fiber::Fiber(function<void()>cb,size_t stacksize):
      id(++s_fiber_id),
      cd(cb){
        ++s_fiber_count;
        //如果stacksize为0就取配置文件中的size
        m_stacksize=stacksize?stacksize:g_stack_size->getval();
        m_stack=MallocStack::Alloc(m_stacksize);
        if(getcontext(&m_ctx)){
         //打印当前栈信息
          ASSERT2(false,"getcontext");
        }
        m_ctx.uc_link=NULL;
        m_ctx.uc_stack.ss_sp=m_stack;
        m_ctx.uc_stack.ss_size=m_stacksize;
        makecontext(&m_ctx,&MainFunc,0);
    }
Fiber::~Fiber(){
      --s_fiber_count;
      if(m_stack){
        ASSERT(m_state==TERM||m_state==INIT);
        MallocStack::Dealloc(m_stack,m_stacksize);     
      }else{
        ASSERT(!cd);
        ASSERT(m_state==EXEC);
        Fiber*cur=t_fiber;
        if(cur==this){
          SetThis(NULL);
        }
      }
      cout<<"fiber:~fiber id="<<id<<endl;;
    }
 void Fiber::reset(function<void()>cd){
      //确保协程有栈空间
      ASSERT(m_stack);
      //状态为TERM或者INIT
      ASSERT(m_state==INIT||m_state==TERM||m_state==EXCEP);
      this->cd=cd;

        if(getcontext(&m_ctx)){
         //打印当前栈信息
          ASSERT2(false,"getcontext");
        }
        m_ctx.uc_link=NULL;
        m_ctx.uc_stack.ss_sp=m_stack;
        m_ctx.uc_stack.ss_size=m_stacksize;
        makecontext(&m_ctx,&Fiber::MainFunc,0);
        m_state=INIT;
    }
    //-将当前协程和正在执行的协程交换
    void Fiber::SwapIn(){
      SetThis(this);
      ASSERT(m_state!=EXEC);
      m_state=EXEC;
      if(swapcontext(&s_fiber->m_ctx,&m_ctx)){

          ASSERT2(false,"swapcontext");
      }
    }
    //设置当前线程
    void Fiber::SetThis(Fiber*f){
      t_fiber=f;
    }
    //协程交出执行全，当前协程切换到后台  子协程------>主协程
    void Fiber::swapOut(){
      SetThis(s_fiber.get());
      if(swapcontext(&m_ctx,&s_fiber->m_ctx)){
          ASSERT2(false,"swapcontext");
      }
    }
    //返回当前执行的协程
     Fiber::ptr Fiber::GetThis(){
      if(t_fiber){
        return t_fiber->shared_from_this();
      }
      Fiber::ptr main_fiber(new Fiber());
      ASSERT(main_fiber.get()==t_fiber);
      s_fiber=main_fiber;
      return t_fiber->shared_from_this();
    }
    //让出执行权(协程切换到后台)并设置状态尾ready
    void Fiber::YieIdToReady(){
      Fiber::ptr cur=GetThis();
      cur->m_state=READY;
      cur->swapOut();
    }
    //让出执行权并设置状态尾Hold
     void Fiber::YieIdToHold(){

      Fiber::ptr cur=GetThis();
      cur->m_state=HOLD;
      cur->swapOut();
    }
    //协程数
    static  uint64_t TotalFibers(){
      return s_fiber_count;
    }
   void Fiber::MainFunc(){
      Fiber::ptr cur=GetThis();
      ASSERT(cur);
      try{
        cur->cd();
        cur->cd=NULL;
        cur->m_state=TERM;

      }catch(exception &e){
        cur->m_state=EXCEP;
        cout<<"fiber EXCEP"<<e.what();
      }catch(...){

        cur->m_state=EXCEP;
        cout<<"fiber EXCEP";
      }
      auto  raw_ptr=cur.get();
      cur.get();
      raw_ptr->swapOut();
    }
#endif
