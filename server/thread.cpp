#include"addlc.hpp"
#include"thread.h"
Logger::ptr logger=LOG_NAME("system");
Thread:: Thread(function<void()> _cd,string name):
      cd(_cd),m_name(name){
        if(name.empty()){
          m_name="UNKNOW";
        }
        //创建线程
      int tid=  pthread_create(&m_thread,NULL,run,(void*)this);
      if(tid<0){
        LOG_ERROR(logger)<<"pthread_create error, tid="<<tid<<",name="<<m_name;
        throw logic_error("pthread_create error");
      }
      //首先确保创建线程是按照顺序进行得，还有确保所有线程创建完再去执行任务
      m_sem.wait();
    }
