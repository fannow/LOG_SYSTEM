#ifndef _UNTIL_H_
#define _UNTIL_H_
#include<pthread.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<vector>
#include<sstream>
#include<cstdint>
#include<iostream>
#include<execinfo.h>
using namespace std;
//获取线程id
pid_t GetThreadId() {
    return syscall(SYS_gettid);
}
uint32_t GetFiberId(){
  return 0;
}
//方便调试
//拿到函数对应的栈信息
void Backtrace(vector<string>&vt,int size,int skip=1){
  void**array=(void**)malloc(sizeof(void*)*size);//生成一个指向栈的指针
  size_t s=::backtrace(array,size);
  char** strings =backtrace_symbols(array,s);
  if(strings==NULL){
    cout<<"backtrace_symbols error"<<endl;
    free(array);
   return;
  }
  //将栈的每一层数据放到数组中去
  for(size_t i=skip;i<s;++i){
    vt.push_back(strings[i]);
  }
  free(array);
  free(strings);
}
string BacktraceTostring(int size,int skip=2,string prefix=""){
  vector<string>vt;
  Backtrace(vt,size,skip);
  stringstream ss;
  for(size_t i=0;i<vt.size();i++){
    ss<<prefix<<vt[i]<<endl;
  }
  return ss.str();
}
#endif // !__UNTIL_H__
