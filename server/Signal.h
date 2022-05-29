#pragma once
#pragma once
#ifndef __SIGNAL_H__
#define __SIGNAL_H__

template<class T, class X = void, int N = 0>
class Signall {
public:
	static T* GetInstance() {

		static T v;
		return &v;
	}
};
//template<class T, class X = void, int N = 0>
//class Signalle {
//public:
//	static shared_ptr<T> GetInstance() {
//		static shared_ptr<T> v(new T);
//		return v;
//	}
//};
#endif // !__SIGNAL_H__
