#ifndef _LOG_HPP_
#define _LOG_HPP_
#include"Log.h"
/******************日志格式控制单元***********************/
class Messageitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	Messageitemforma(string str = "") {}

	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->getss();
	}

};
class Levelitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	Levelitemforma(string str = "") {}

	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << LogLevel::Tostring(level);
	}

};
//程序启动后消耗时间
class Pleaseitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	Pleaseitemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->getplease();
	}

};
class lognameitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	lognameitemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->GetLogger()->getname();
	}
};
//线程id
class ThreadIditemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	ThreadIditemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getthreadid();
	}
};
//协程id
class fiberiditemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	fiberiditemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->getfiberid();
	}
};
//运行时间
class Timeitemforma :public Formater::Item {
private:
	//时间格式
	string time_formate;
public:
	typedef shared_ptr<Messageitemforma> ptr;
	Timeitemforma(string format = "%Y:%m:%d %H:%M:%S") :
		time_formate(format) {
		if (time_formate.empty()) {
			time_formate = "%Y:%m:%d %H:%M:%S";
		}
	}


	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		//日志时间格式设置
		struct tm* tm;
		time_t t = event->gettime();
		tm = localtime(&t);
		char buff[64];
		strftime(buff, sizeof(buff), time_formate.c_str(), tm);
		os << buff;
	}

};


//文件名称
class filenameitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	filenameitemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->getfile();
	}
};
//文件行号
class lineitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	lineitemforma(string str = "") {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << event->getline();
	}
};


class stringitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	stringitemforma(string format) :
		str(format) {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << str;
	}
private:

	string str;
};
class tabitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	tabitemforma(string format) {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << '\t';
	}
private:
};

//换行
class newlineitemforma :public Formater::Item {
public:
	typedef shared_ptr<Messageitemforma> ptr;
	newlineitemforma(string format) :
		str(format) {}
	void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) override {
		os << endl;
	}
private:
	//时间格式
	string str;
};

/*****************日志级别***********************/
const char* LogLevel::Tostring(LogLevel::Level level) {
	switch (level) {
		/*case UNKNOW:
			return "UNKNOW";
			break;
		*/
#define XX(name)\
	case name:\
	return #name;\

		XX(DEBUG);
		XX(WARN);
		XX(INFO);
		XX(ERROR);
		XX(FATAL);
#undef XX
	default:
		return "UNKNOW";
	}
	return "UNKNOW";
}
/*****************日志事件***********************/

LogEvent::LogEvent(shared_ptr<Logger> _ptr, LogLevel::Level level, const char* file,
	int32_t line, uint32_t please, uint32_t threadid, uint32_t fiberid, uint32_t time)
	:logger(_ptr), level(level), m_file(file), m_line(line), m_please(please),
	m_threadid(threadid), m_fiberId(fiberid), m_time(time) {
}

void LogEvent::format(const char* fmt, ...) {
	va_list al;
	va_start(al, fmt);
	format(fmt, al);
	va_end(al);

}
void LogEvent::format(const char* fmt, va_list al) {
	char* buff = NULL;
	int len = vasprintf(&buff, fmt, al);

	if (len != -1) {
		m_ss << string(buff, len);
	}
	delete buff;
}





/*****************日志格式***********************/

Formater::Formater(string pattern) :
	m_pattern(pattern) {
	Formater::Init();
}

string Formater::format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
	stringstream ss;

	for (auto& i : m_items) {
		//使用内部类派生类进行日志解析
		i->format(ss, logger, level, event);
	}
	return ss.str();

}
/*
*内层循环从%下一个字符开始遍历。同时标记为状态0
*如果在状态0的情况下遇到{标记为状态1，获取%到{之间的子串如果在状态1的情况下遇到}标记为状态2。获取{到}之间的子串
*从目前看如果内层循环所有条件都不满足时会陷入死循环，而且下面push的vec元素类型也不匹配
*/
void Formater::Init() {

	//tuple(str,fmt,type)
	vector<tuple<string, string, int>>vec;

	string nstr;
	for (size_t i = 0;i < m_pattern.size();i++) {
		if (m_pattern[i] != '%') {
			nstr.append(1, m_pattern[i]);
			continue;
		}
		//其实还是一个%
		if ((i + 1) < m_pattern.size()) {
			if (m_pattern[i + 1] == '%') {
				nstr.append(1, '%');
				continue;
			}
		}
		size_t n = i + 1;
		int fmt_staus = 0;
		int fmt_beign = 0;


		string str;
		string fmt;
		while (n < m_pattern.size()) {
			//遇见空格
			if (!isalpha(m_pattern[n]) && m_pattern[n] != '}' && m_pattern[n] != '{') {
				str = m_pattern.substr(i + 1, n - i - 1);
				break;
			}
			if (fmt_staus == 0) {
				if (m_pattern[n] == '{') {
					//字符串分割
					str = m_pattern.substr(i + 1, n - i - 1);
					fmt_staus = 1;
					fmt_beign = n;
					n++;
					continue;
				}

			}
			else if (fmt_staus == 1) {
				if (m_pattern[n] == '}') {
					//字符串分割
					fmt = m_pattern.substr(fmt_beign + 1, n - fmt_beign - 1);
					//	cout << "#" << fmt << endl;
					fmt_staus = 0;
					++n;
					break;
				}

			}
			++n;
			if (n == m_pattern.size()) {
				if (str.empty()) {
					str = m_pattern.substr(i - 1);
				}
			}
		}

		if (fmt_staus == 0) {
			if (!nstr.empty()) {
				vec.push_back(make_tuple(nstr, string(), 0));
				nstr.clear();
			}
			str = m_pattern.substr(i + 1, n - i - 1);

			//解析日志将日志存储在数组中

			vec.push_back(make_tuple(str, fmt, 1));
			i = n - 1;
		}
		else if (fmt_staus == 1) {
      m_error=true;
			cout << "pattern parse error" << m_pattern << "-" << m_pattern.substr(i) << endl;
			//格式错误
			vec.push_back(make_tuple("<<pattern_error>>", fmt, 0));
		}
	}
	if (!nstr.empty()) {
		vec.push_back(make_tuple(nstr, "", 0));

	}

	/*
	* %m--->消息体
	* %p--->日志级别
	* %r--->启动的时间
	* %c--->日志名称
	* %t--->线程id
	* %n--->回车
	* %d--->时间
	* %f--->文件名
	* %l--->行号
	*/

	//function<Item::ptr(string str)> 返回值为iter::ptr 参数为string得函数
	static map<string, function<Formater::Item::ptr(string str)>> formar_item = {

#define xx(str,c) \
		{ #str,[](string fmt) {return Formater::Item::ptr(new c(fmt));}}

		xx(m, Messageitemforma),
		xx(p, Levelitemforma),
		xx(r, Pleaseitemforma),
		xx(c, lognameitemforma),
		xx(t, ThreadIditemforma),
		xx(n, newlineitemforma),
		xx(d, Timeitemforma),
		xx(f, filenameitemforma),
		xx(l, lineitemforma),
		xx(T, tabitemforma),
		xx(F, fiberiditemforma),

#undef xx

	};
	for (auto& i : vec) {
		if (get<2>(i) == 0) {
			m_items.push_back(Formater::Item::ptr(new stringitemforma(get<0>(i))));
		}
		else {
			auto it = formar_item.find(get<0>(i));
			if (it == formar_item.end()) {
        m_error=true;
				m_items.push_back(Formater::Item::ptr(new stringitemforma("<<error_format %" + get<0>(i) + ">>")));
			}
			else {
				m_items.push_back(it->second(get<1>(i)));

			}

		}

	}

}


/********************日志器********************/
void Logger::AddAppender(LogAppender::ptr appender)//添加
{
    SpinLock::Lock lock(m_mutex);
	if (!appender->GetFormat()) {
		//设置日志格式
    SpinLock::Lock ll(appender->m_mutex);
		appender->m_format=format;
	}
	m_appender.push_back(appender);
}
void Logger::DelAppendrt(LogAppender::ptr appender)//删除
{
    SpinLock::Lock lock(m_mutex);
	//遍历链表寻找appender
	for (auto it = m_appender.begin();it != m_appender.end();it++) {
		if (*it == appender) {
			//直接删除就可以
			m_appender.erase(it);
			break;
		}
	}
}

Logger::Logger(string _name) :
	m_name(_name), m_level(LogLevel::DEBUG) {
	//格式控制
	format.reset(new Formater("%d%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m %n"));
}
void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {
	if (level >= m_level) {
		//将自己作为智能指针传入到函数中去
		auto self = shared_from_this();
    SpinLock::Lock lock(m_mutex);
    if(!m_appender.empty()){
	  	for (auto& i : m_appender) {
	  		i->Log(self, level, event);
	  	}
    }else if(root){
     root->Log(level,event);
    }
	}
}
//日志级别
void  Logger::debug(LogEvent::ptr event) {

	Log(LogLevel::DEBUG, event);
}
void  Logger::info(LogEvent::ptr event) {
	Log(LogLevel::INFO, event);
}
void  Logger::warn(LogEvent::ptr event) {
	Log(LogLevel::WARN, event);
}
void  Logger::error(LogEvent::ptr event) {
	Log(LogLevel::ERROR, event);
}

void  Logger::fatal(LogEvent::ptr event) {
	Log(LogLevel::FATAL, event);
}

/**********************************日志输出器LogAppender 成员函数实现****************************************************/
//子类fileAppender类函数实现
/*仅在类内部起作用的类型别名*///复写（即override）虚函数。
void FileAppender::Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
	if (level >= m_level) {
    uint64_t now=time(0);
    if(now!=m_lasttime){//最后一次时间不对就重新打开文件
      repoen();
      m_lasttime=now;
    }
		//将对应日志输出到文件中
    SpinLock::Lock lock(m_mutex);
    if(!(m_filestream << m_format->format(logger, level, event))){
      cout<<"fileappender log error"<<endl;
    }
		/*ofstream是从内存到硬盘*/
	}
}
//日志文件重新打开
bool  FileAppender::repoen() {
  SpinLock::Lock lock(m_mutex);
	if (m_filestream) {
		//先关闭文件
		m_filestream.close();
	}
	//在打开文件
	m_filestream.open(m_file_name.c_str());
	//双重否定将int类型转换为bool类型
	//得到原先数值得bool类型
	return !!m_filestream;

}
//子类stdoutAppender实现类函数
void StdOutAppender::Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    SpinLock::Lock lock(m_mutex);
	if (level >= m_level) {

		//将对应的日志时间设置为输出日志格式
		cout << m_format->format(logger, level, event);
	}
}
#endif
