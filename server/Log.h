#ifndef _LOG_H_
#define _LOG_H_
#include<iostream>
#include<string>
#include<tuple>
#include<memory>
#include<vector>
#include<string>
#include<list>
#include<sstream>
#include<fstream>
#include<string.h>
#include<map>
#include<functional>
#include<time.h>
#include<cstdio>
#include<map>
#include<stdarg.h>
#include<yaml-cpp/yaml.h>
#include"Signal.h"
#include"thread.h"
#include"Until.h"
using namespace std;
class Formater;
class Logger;
class LogMessage;

/*
* 用于控制台输出宏
*/
#define LOG_LEVEL(logger, level) \
    if(logger->getlevel() <= level) \
			LogEventWarp(LogEvent::ptr(new LogEvent(logger, level,__FILE__,\
			__LINE__, 0, GetThreadId(),GetFiberId(), time(0)))).getSS()
#define LOG_DEBUG(logger) LOG_LEVEL(logger, LogLevel::DEBUG)
#define LOG_ERROR(logger) LOG_LEVEL(logger, LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, LogLevel::FATAL)
#define LOG_INFO(logger) LOG_LEVEL(logger, LogLevel::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, LogLevel::WARN)




/*
* 参数说明：__VA_ARGS__:用于在宏替换部分中，表示可变参数列表；
* 用于文件输出宏
*
*/
#define FMT_LOG_LEVEL(logger, level,fmt,...) \
    if(logger->getlevel() <= level) \
			LogEventWarp(LogEvent::ptr(new LogEvent(logger, level,__FILE__,\
			__LINE__, 0, 1,2, time(0)))).getevent()->format(fmt,__VA_ARGS__)



#define FMT_LOG_DEBUG(logger,fmt,...) FMT_LOG_LEVEL(logger, LogLevel::DEBUG,fmt,__VA_ARGS__)
#define FMT_LOG_ERROR(logger,fmt,...) FMT_LOG_LEVEL(logger, LogLevel::ERROR,fmt,__VA_ARGS__)
#define FMT_LOG_FATAL(logger,fmt,...) FMT_LOG_LEVEL(logger, LogLevel::FATAL,fmt,__VA_ARGS__)
#define FMT_LOG_INFO(logger,fmt,...) FMT_LOG_LEVEL(logger, LogLevel::INFO,fmt,__VA_ARGS__)
#define FMT_LOG_WARN(logger,fmt,...) FMT_LOG_LEVEL(logger, LogLevel::WARN,fmt,__VA_ARGS__)


#define LOG_ROOT()  LogRoot::GetInstance()->getroot()


#define LOG_NAME(name)  LogRoot::GetInstance()->getlogger(name)


/*
* 日志级别
*
*/
class LogLevel {
public:
	typedef shared_ptr<LogLevel> ptr;
	enum Level {
		UNKNOW = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
	};
	static const char* Tostring(LogLevel::Level level);
  static LogLevel::Level Formstring(string str){
#define XX(level,v)\
    if(str==#v){\
      return LogLevel::level;\
    }

    XX(DEBUG,debug);
    XX(INFO,info);
    XX(WARN,warn);
    XX(ERROR,error);
    XX(FATAL,fatal);
    XX(DEBUG,DEBUG);
    XX(INFO,INFO);
    XX(WARN,WARN);
    XX(ERROR,ERROR);
    XX(FATAL,FATAL);
      return LogLevel::UNKNOW;
#undef XX

  }
};

/*
* 日志事件
* 成员变量：
* 	const char* m_file = NULL;			 文件名
*	int32_t m_line = 0;				     行号
*	uint32_t m_please = 0;               程序运行时间（毫秒）
*	uint32_t m_threadid = 0;             线程id
*	uint32_t m_fiberId = 0;              协程id
*	uint64_t m_time = 0;                 时间戳
*	stringstream m_ss;				     流式格式
* 函数说明：
* vasprintf()函数:int vasprintf (char **buf, const char *format, va_list ap)将格式化数据从可变参数列表写入缓冲区
*
*/

class LogEvent {
public:
	//默认构造函数
	LogEvent() {};
	LogEvent(shared_ptr<Logger> _ptr, LogLevel::Level level, const char* file, int32_t line,
		uint32_t please, uint32_t threadid, uint32_t fiberid, uint32_t time);

	typedef shared_ptr<LogEvent> ptr;

	const char* getfile() { return m_file; } //返回文件名字
	int32_t getline() { return m_line; }      //返回行号
	uint32_t getplease() { return m_please; };//返回程序运行时间
	uint32_t getthreadid() { return m_threadid; }//返回线程id
	uint32_t getfiberid() { return m_fiberId; }//返回协程id
	uint64_t gettime() { return m_time; }//返回现在时间
	string getss() { return m_ss.str(); }//返回流式
	shared_ptr<Logger> GetLogger() { return logger; }
	stringstream& getSS() { return m_ss; }  //返回流式输出
	LogLevel::Level GetLevel() { return level; }//获取日志级别


	void format(const char* fmt, ...);	//格式控制
	void format(const char* fmt, va_list al);//格式控制   va_list:可变参数列表


private:

	shared_ptr<Logger> logger;			//日志器	
	LogLevel::Level level;				//日志级别
	const char* m_file = NULL;			//文件名
	int32_t m_line = 0;				    //行号
	uint32_t m_please = 0;              //程序运行时间（毫秒）
	uint32_t m_threadid = 0;            //线程id
	uint32_t m_fiberId = 0;             //协程id
	uint64_t m_time = 0;                //时间戳
	stringstream m_ss;				    //流式格式		

};





/*
*日志控制器
*/
class Formater {
public:
	typedef shared_ptr<Formater> ptr;
	Formater(string pattern);
	string format(shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event);
public:
	/*
	* 日志控制单元
	* 格式控制子模块负责具体的格式分析
	*/
	class Item {
	public:
		typedef shared_ptr<Item> ptr;
		virtual ~Item() {}

		virtual void format(ostream& os, shared_ptr<Logger> logger, LogLevel::Level leve, LogEvent::ptr event) = 0;
	};

	void Init();//初始化  分析用户输入要输出的日志格式
  bool IsError(){return m_error;}
  string Tostring(){
    return m_pattern;
  }
private:
	string m_pattern;        //
	vector<Item::ptr> m_items;

  bool m_error=false;//判断是否有错
};


//日志输出
class LogAppender {
  friend class Logger;
public:

	typedef shared_ptr<LogAppender> ptr;
	virtual~LogAppender() {}
	virtual void Log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;//日志输出分别调用不同的控制器的Log输出到不同的地方
	Formater::ptr GetFormat() {
    SpinLock::Lock lock(m_mutex);
    return m_format; 
  }  //返回日志格式
	void setlevel(LogLevel::Level level) { m_level = level; }//设置日志级别
	LogLevel::Level GetLevel() { return m_level; }//返回日志界别
  virtual string toyamlstring()=0;
  //添加输出地的格式
  void SetFormat(Formater::ptr val){
    SpinLock::Lock lock(m_mutex);
    m_format=val;
    if(m_format){
      hasformater=true;
    }else{
      hasformater=false;
    }

  }
protected:
  bool hasformater=false;
	LogLevel::Level m_level;//日志级别
	Formater::ptr m_format;//输出日志格式//将用于输出日志
  SpinLock m_mutex;
};



/*
*日志器
*/
class Logger :public enable_shared_from_this<Logger> {
  friend class LogMessage;
public:
	typedef shared_ptr<Logger> ptr;
	Logger(string _name = "root");
	void Log(LogLevel::Level level, LogEvent::ptr event);

	void AddAppender(LogAppender::ptr appender);//添加日志输出地
	void DelAppendrt(LogAppender::ptr appender);//删除日志输出地

	/*
	*日志级别
	*/
	void debug(LogEvent::ptr event);
	void info(LogEvent::ptr event);
	void warn(LogEvent::ptr event);
	void error(LogEvent::ptr event);
	void fatal(LogEvent::ptr event);

  void ClearAppender(){
    SpinLock::Lock lock(m_mutex);
    m_appender.clear();
  }
	LogLevel::Level getlevel() { return m_level; }//返回日志级别
	string getname()const { return m_name; } //返回日志名称


	void setleve(LogLevel::Level level) { m_level = level; }
	void setformat(Formater::ptr fmt) { 
    SpinLock::Lock lock(m_mutex);
    format = fmt; 
    for(auto &i:m_appender){
    SpinLock::Lock ll(i->m_mutex);
      if(!i->hasformater){
        i->m_format=format;
      }  
    }
  }
  void SetFomater(string formater){
    Formater::ptr new_val(new Formater(formater));
    if(new_val->IsError()){
      cout<<"Logger SetFormater name="<<m_name<<
        "value="<<formater<<"invalid formater"<<endl;;
      return;
    }
        setformat(new_val);
  }
  
  Formater::ptr getformat(){
    SpinLock::Lock lock(m_mutex);
    return format;
  }
  string toyamlstring(){
    SpinLock::Lock lock(m_mutex);
    YAML::Node n;
    n["name"]=m_name;
    n["level"]=LogLevel::Tostring(m_level);
     if(format){
      n["formater"]=format->Tostring();
    }
     for(auto &o:m_appender){
       n["addernders"].push_back(YAML::Load(o->toyamlstring()));

     }
     stringstream ss;
     ss<<n;
     return ss.str();
  }
private:
  SpinLock m_mutex;
	string m_name;             //日志名称
	LogLevel::Level m_level;	  //日志级别（满足这各级别就会输出）
	list<LogAppender::ptr> m_appender;  //日志列表

	Formater::ptr format;  //日志格式
  Logger::ptr root;
};



/*
* 控制台输出器
* override仅在类内部起作用的类型别名复写（即虚函数）。
*/
class StdOutAppender :public LogAppender {
public:
	typedef shared_ptr<StdOutAppender> ptr;
	virtual void Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)override;//输出到控制台
  string toyamlstring(){
    SpinLock::Lock lock(m_mutex);
    YAML::Node n;
    n["type"]="StdOutAppender";
    if(m_level!=LogLevel::Level::UNKNOW){
      n["level"]=LogLevel::Tostring(m_level);
    }
    if(hasformater&&m_format){
       n["formater"]=m_format->Tostring();
    }
    stringstream ss;
    ss<<n;
    return ss.str();
  }
private:
};


/*
* 文件输出器
*/
class FileAppender :public LogAppender {
public:
	typedef shared_ptr<FileAppender> ptr;

	FileAppender(string filename) :
		m_file_name(filename) {
		//先打开文件
		repoen();

	}

	void Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;//输出到文件


	bool repoen();//重新打开日志文件
  string toyamlstring(){
    SpinLock::Lock lock(m_mutex);
    YAML::Node n;
    n["type"]="FileAppender";
    n["file"]=m_file_name;
    if(m_level!=LogLevel::Level::UNKNOW){
      n["level"]=LogLevel::Tostring(m_level);
    }
    if(hasformater&&m_format){
       n["formater"]=m_format->Tostring();
    }
    stringstream ss;
    ss<<n;
    return ss.str();
  }
private:
  uint64_t m_lasttime=0;
	string m_file_name;//日志文件
	ofstream m_filestream;//文件流
};

/*
* 日志管理器
*/
class LogMessage {
  SpinLock m_mutex;
public:
	LogMessage() {
		//默认为stdout
		root.reset(new Logger());
		root->AddAppender(LogAppender::ptr(new StdOutAppender()));
    mp[root->m_name]=root;
    Init();
	}
	map<string, Logger::ptr> mp;
	Logger::ptr root;
public:
	Logger::ptr getlogger(string name) {
    SpinLock::Lock lock(m_mutex);
		auto it = mp.find(name);
    if(it!=mp.end()){
		return it->second;
  	}
    //不存在创建一个logger
    Logger::ptr logger(new Logger(name));
    logger->root=root;
    mp[name]=logger;
    return logger;
  }
	void Init(){}
	Logger::ptr getroot() { return root; }
  string toyamlstring(){
    SpinLock::Lock lock(m_mutex);
    YAML::Node node;
    for( auto &i: mp ){
      node.push_back(YAML::Load(i.second->toyamlstring()));
    }
    stringstream ss;
    ss<<node;
    return ss.str();
  }

};
/*
* 日志事件包装器
*
* LogEventWarp作为一个零时对象，在使用完后直接析构，触发日志直接写入，
*然而日志本身是智能指针，如果在声明在主函数里面，程序不结束将无法释放
*/
class LogEventWarp {
public:

	LogEventWarp(LogEvent::ptr e)
		:m_event(e) {}
	~LogEventWarp() {
		m_event->GetLogger()->Log(m_event->GetLevel(), m_event);
	}
	shared_ptr<LogEvent>	getevent() { return m_event; }
	//获取日志内容流
	stringstream& getSS() { return m_event->getSS(); }
private:
	//日志事件
	shared_ptr<LogEvent> m_event;


};

typedef Signall<LogMessage> LogRoot;

#endif
