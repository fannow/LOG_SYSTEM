#ifndef _THREAD_H_
#define _THREAD_H_
#include<thread>
#include<iostream>
#include<functional>
#include<memory>
#include<semaphore.h>
#include<pthread.h>
#include<cstdio>
#include"Until.h"
using namespace std;
//模板锁结构体
//构造用于加锁
//析构由于解锁
template<class T>
struct LockImpl{
  LockImpl(T&_mutex):
  m_mutex(_mutex)
  {
    m_mutex.lock();//加锁
    m_locked=true;//更新状态
  }
  ~LockImpl(){
    unlock();
  }
  //加锁
  void lock(){
    //防止死锁
    if(!m_locked){
      m_mutex.lock();
      m_locked=true;
    }
  }
  //解锁
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked=false;
    }
  }
  private:
    T&m_mutex;
    bool m_locked;
  
};
//读信号量
template<class T>
struct ReadLockImpl{
  ReadLockImpl(T&_mutex):
  m_mutex(_mutex)
  {
    m_mutex.rdlock();//加锁
    m_locked=true;//更新状态
  }
  ~ReadLockImpl(){
    unlock();
  }
  //加锁
  void lock(){
    //防止死锁
    if(!m_locked){
      m_mutex.rdlock();
      m_locked=true;
    }
  }
  //解锁
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked=false;
    }
  }
  private:
    T&m_mutex;
    bool m_locked;  
};
//写信号量
template<class T>
struct WriteLockImpl{
  WriteLockImpl(T&_mutex):
  m_mutex(_mutex)
  {
    m_mutex.wrlock();//加锁
    m_locked=true;//更新状态
  }
  ~WriteLockImpl(){
    unlock();
  }
  //加锁
  void lock(){
    //防止死锁
    if(!m_locked){
      m_mutex.wrlock();
      m_locked=true;
    }
  }
  //解锁
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked=false;
    }
  }
  private:
    T&m_mutex;
    bool m_locked;
  
};
//读写锁
class rwMutex{
  private:
  pthread_rwlock_t m_lock;
  public:
  typedef ReadLockImpl<rwMutex> readmutex;
  typedef WriteLockImpl<rwMutex> writemutex;
  rwMutex(){
    pthread_rwlock_init(&m_lock,NULL);
  }
  ~rwMutex(){
    pthread_rwlock_destroy(&m_lock);
  }
  void rdlock(){
  
    pthread_rwlock_rdlock(&m_lock);
  }
  void wrlock(){

    pthread_rwlock_wrlock(&m_lock);
  }
  void unlock(){
    
    pthread_rwlock_unlock(&m_lock);
  }

};
//普通锁
class Mutex{
  private:
    pthread_mutex_t m_mutex;
  public:
    typedef LockImpl<Mutex> Lock;
    Mutex(){
    pthread_mutex_init(&m_mutex,NULL);
    }
    ~Mutex(){
      pthread_mutex_destroy(&m_mutex);

    }
    void lock(){
      pthread_mutex_lock(&m_mutex);

    }
    void unlock(){
      pthread_mutex_unlock(&m_mutex);
    }

};
class SpinLock{
  private:
    pthread_spinlock_t m_mutex;
  public:
 typedef   LockImpl<SpinLock> Lock;
    SpinLock(){
      pthread_spin_init(&m_mutex,0);
    }
    ~SpinLock(){
      
      pthread_spin_destroy(&m_mutex);
    }
    void lock(){

      pthread_spin_lock(&m_mutex);
    }
    void unlock(){

      pthread_spin_unlock(&m_mutex);
    }
};
//信号量
class Semaphore{
  private:
    sem_t m_set;
      
  private:
    Semaphore(const Semaphore&)=delete;
    Semaphore(const Semaphore&&)=delete;
    Semaphore operator=(const Semaphore&)=delete;
  public:
    Semaphore(uint32_t count=0){
      //信号量初始化
      if(sem_init(&m_set,0,count)){
        throw logic_error("sem_init error");

      }
    }
    ~Semaphore(){
      sem_destroy(&m_set);
    }
    void wait(){
      if(sem_wait(&m_set)){
       throw logic_error("sem_wait error");
      }          
            
    }
    void notify(){
      if(sem_post(&m_set)){
       throw  logic_error("sem_post error");
      }
    }

};
class Thread;
static thread_local Thread* t_pthread=NULL;
static thread_local string t_pthread_name="UNKNOW";
//协程是在线程上跑
//线程是作为协程得容器
class Thread{
  public:
    typedef shared_ptr<Thread> ptr;
 Thread(function<void()> _cd,string name):
      cd(_cd),m_name(name){
        if(name.empty()){
          m_name="UNKNOW";
        }
        //创建线程
      int tid=  pthread_create(&m_thread,NULL,run,(void*)this);
      if(tid<0){
        cout<<"pthread_create error, tid="<<tid<<",name="<<m_name;
        throw logic_error("pthread_create error");
      }
      //首先确保创建线程是按照顺序进行得，还有确保所有线程创建完再去执行任务
      m_sem.wait();
    }
    //线程执行函数
    static void *run(void *rid){
          Thread *thread=(Thread*)rid; 
          t_pthread=thread;
          SetName(t_pthread->getName());
          thread->tid=GetThreadId();
          pthread_setname_np(pthread_self(),thread->m_name.substr(0,15).c_str());
          function<void()> cd;
          cd.swap(thread->cd);
          thread->m_sem.notify();//信号量++确保线程执行任务(等到所有线程初始化并跑起来，再去执行任务)
          //执行回调函数---任务函数
          cd();
         return 0;
    }
    string getName(){return m_name;}
    pid_t getId(){return tid;}
    void join(){
      if(m_thread){
       int rid= pthread_join(m_thread,NULL);
       if(rid<0){
       // LOG_ERROR(logger)<<"pthread_join error, rid="<<rid;
       cout<<"pthread_join error rid="<<rid<<endl;
        throw logic_error("pthread_join error");
       }
       m_thread=0;
      }
    }
    //获取当前线程
    static Thread*getThis(){
      return t_pthread;
    }
    //获取当前线程名称
    static string GetName(){
      return t_pthread_name;
    }
    static void SetName(const string &name){
      if(t_pthread){
        t_pthread->m_name=name;
      }
      t_pthread_name=name;
    }
    ~Thread(){
      if(m_thread){
        pthread_detach(m_thread);
      }
    }
  private:
   //因为互斥量和互斥信号量是不能拷贝的
    Thread(const Thread&)=delete ;
    Thread(const Thread&&)=delete ;
    Thread&  operator=(const Thread&)=delete;
  private:
    pid_t tid; //线程id
    string m_name="UNKONW";//线程名字
    function<void()> cd;
    pthread_t m_thread;
    Semaphore m_sem;
};
#endif
