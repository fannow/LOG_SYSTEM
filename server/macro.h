#ifndef _MACRO_H_
#define _MACRO_H_
#include<string>
#include<assert.h>
#include"addlc.hpp"
#include"Until.h"

/*
 *输出当前栈的信息
 *
*/
#define ASSERT(x) \
  if(!(x)){\
    LOG_ERROR(LOG_ROOT())<<"ASSERTON: " #x \
      <<"\nbacktrace:\n" \
      <<BacktraceTostring(100,2,"   ");\
    assert(x);\
  }
#define ASSERT2(x,w) \
  if(!(x)){\
    LOG_ERROR(LOG_ROOT())<<"ASSERTON: " #x \
      <<"\n"<<w\
      <<"\nbacktrace:\n" \
      <<BacktraceTostring(100,2,"   ");\
    assert(x);\
  }

#endif
