#ifndef __CONFIG_H__
#define __CONFIG_H__
#include<iostream>
#include<vector>
#include<string>
#include <typeinfo>
#include<unordered_set>
#include<map>
#include<unordered_map>
#include<memory>
#include<yaml-cpp/yaml.h>
#include<functional>
#include<sstream>//序列化
#include"thread.h"
#include <boost/lexical_cast.hpp>
#include"Log.hpp"
/*
 *通过配置文件配置配置项
 *配置系统原则 约定由于配置
 * */
using namespace std;
class person{
  public:
    person(){}
    string name="lisan";
    string id="1234";
    int age=0;
  public:
    string tostring ()const{
      stringstream ss;
      ss<<"[name="<<name
        <<"id="<<id
        <<"age="<<age
        <<"]";
      return ss.str();
    }
    bool operator==(const person&p) const{
      return name==p.name&&id==p.id&&age==p.age;
    }
};
//配置系统yaml
//公用属性
class ConfigVarBase {
public:
	typedef shared_ptr<ConfigVarBase> ptr;
	ConfigVarBase(string name, string descripation = "") :
    m_name(name), m_description(descripation) {
      /*
       * 大小写转换都转成小写
       *
       * */
      transform(m_name.begin(),m_name.end(), m_name.begin(),::tolower);
    }
	virtual ~ConfigVarBase() {}
	string getname() { return m_name; };
	string getdescription() { return m_description; }

	virtual string Tostring() = 0;
	virtual bool fromstring(string val) = 0;
  virtual string getTypeName()=0;
protected:
	string m_name;
	string m_description;


};
//基础类型转换从类型F转换为T
//满足基础类型转换
template<class F,class T>
class LexicalCast{
  public:
    T operator()(F f){
      return boost::lexical_cast<T>(f);
    }
     
};
//基础容器的偏特化
template<class T>
class LexicalCast<string,vector<T>>{
  public:
   vector<T> operator()(string str){
     cout<<"vector  反序列化"<<endl;
     stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道vector<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::vector<T> vec;
      for(int i=0;i<root.size();i++){
        ss.str("");
        ss<<root[i];
        vec.push_back(LexicalCast<string,T>()(ss.str()));

      }
      return vec;
    }
     
};
//vector<T>的序列化
template<class T>
class LexicalCast<vector<T>,string>{
  public:
   string operator()(vector<T> &v){
     cout<<"vector  序列化"<<endl;
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
        root.push_back(YAML::Load(LexicalCast<T,string >()(o)));
      }
      ss<<root;
      return ss.str();
    } 
     
};

//list反序列
template<class T>
class LexicalCast<string,list<T>>{
  public:
   list<T> operator()(string &str){
     stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道list<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::list<T> vec;
      for(int i=0;i<root.size();i++){
        ss.str("");
        ss<<root[i];
        vec.push_back(LexicalCast<string,T>()(ss.str()));

      }
      return vec;
    }
     
};
//list<T>的序列化
template<class T>
class LexicalCast<list<T>,string>{
  public:
   string operator()(list<T> &v){
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
        root.push_back(YAML::Load(LexicalCast<T,string >()(o)));
      }
      ss<<root;
      return ss.str();
    } 
};    
//set反序列
template<class T>
class LexicalCast<string,set<T>>{
  public:
   set<T> operator()(string &str){
     stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道list<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::set<T> vec;
      for(int i=0;i<root.size();i++){
        ss.str("");
        ss<<root[i];
        vec.insert(LexicalCast<string,T>()(ss.str()));

      }
      return vec;
    }
     
};
//set<T>的序列化
template<class T>
class LexicalCast<set<T>,string>{
  public:
   string operator()(set<T> &v){
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
        root.push_back(YAML::Load(LexicalCast<T,string >()(o)));
      }
      ss<<root;
      return ss.str();
    } 
};    
//unordered_set反序列
template<class T>
class LexicalCast<string,unordered_set<T>>{
  public:
   unordered_set<T> operator()(string &str){
     stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道list<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::unordered_set<T> vec;
      for(int i=0;i<root.size();i++){
        ss.str("");
        ss<<root[i];
        vec.insert(LexicalCast<string,T>()(ss.str()));

      }
      return vec;
    }
     
};
//unordered_set<T>的序列化
template<class T>
class LexicalCast<unordered_set<T>,string>{
  public:
   string operator()(unordered_set<T> &v){
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
        root.push_back(YAML::Load(LexicalCast<T,string >()(o)));
      }
      ss<<root;
      return ss.str();
    } 
};    
//map反序列
template<class T>
class LexicalCast<string,map<string,T>>{
  public:
   map<string,T> operator()(string str){
     //stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道list<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::map<string,T> vec;
     stringstream ss;
      for(auto it=root.begin();it!=root.end();it++){
        ss.str("");
        cout<<"size" <<typeid(*it).name();
       ss<<it->second;
      vec.insert(make_pair(it->first.Scalar(),LexicalCast<string,T>()(ss.str())));

      }
      return vec;
    }
     
};
//map<T>的序列化
template<class T>
class LexicalCast<map<string,T>,string>{
  public:
   string operator()(map<string,T> &v){
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
      root[o.first]=YAML::Load(LexicalCast<T,string >()(o.second));
      
      }
      ss<<root;
      return ss.str();
    } 
};    
//unordered_map反序列
template<class T>
class LexicalCast<string,unordered_map<string,T>>{
  public:
   unordered_map<string,T> operator()(string &str){
     stringstream ss;
     //将yaml格式的string转化为转换为其他格式
      YAML::Node root=YAML::Load(str);//这里转换为数组
      //模板在实例化之前并不知道list<T>是什么东西，使用typename可以让定义确定下来
      //typename创建类型的别名 是一个类型不是一个变量。
     typename   std::unordered_map<string,T> vec;
      for(auto it=root.begin();it!=root.end();it++){
        ss.str("");
        ss<<it->second;
        vec.insert(make_pair(it->first.Scalar(),LexicalCast<string,T>()(ss.str())));

      }
      return vec;
    }
     
};
//unordered_map<T>的序列化
template<class T>
class LexicalCast<unordered_map<string,T>,string>{
  public:
   string operator()(unordered_map<string,T> &v){
     stringstream ss;
      YAML::Node root;
      for(auto &o:v){
      root[o.first]=YAML::Load(LexicalCast<T,string >()(o.second));
      
      }
      ss<<root;
      return ss.str();
    } 
};
//自定义类型偏特化
template<>
class LexicalCast<string,person>{
  public:
   person operator()(string str){
     YAML::Node node= YAML::Load(str);
     person p;
     p.name=node["name"].as<string>();
     p.age=node["age"].as<int>();
     p.id=node["id"].as<string>();
    return p;
   }
     
};
template<>
class LexicalCast<person,string>{
  public:
   string operator()(person p){
     stringstream ss;
      YAML::Node node;
      node["name"]=p.name;
      node["id"]=p.id;
      node["age"]=p.age;
      ss<<node;
      return ss.str();
    } 
};    
//序列化和反序列化
template<class T,class FormStr=LexicalCast<string,T>,class ToStr=LexicalCast<T,string>>
class ConfigVar :public ConfigVarBase {
public:
  typedef rwMutex RWM;
	typedef shared_ptr<ConfigVar> ptr;
  //回调函数  动态配置-----配置更改时通知原来的值是什么，新的值是什么，根据新的值做一些改变
  typedef std::function<void (const T &old_values,const T& new_value)>on_chang_ck;
 // map<uint64_t,on_chang_ck> m_ck;
  
	ConfigVar(string name, T& val, string description = "")
		: ConfigVarBase(name, description), m_val(val)
	{	}
	string Tostring() {
		try {
      
     RWM::readmutex lock(m_mutex);
      return ToStr()(m_val);  
	//		using boost::lexical_cast;
			//将T 转换为流式string
		//	return lexical_cast<string> (m_val);//将m_val流式处理返回到标准库中字符串的流中，
			//然后作为模板对象输出，这里模板对象就是string（就是将m_val转换为val，存储在string 然后以string形式返回）

		}
		catch (exception& e) {
			/*
			*e.what() 将异常信息打印出来
			*typeid().name()，可以用来返回变量的数据类型
			*
			*/
			LOG_ERROR(LOG_ROOT()) << "ConfigVal::ToString expection" << e.what() << "convert:" << typeid(m_val).name() << "to string";
		}

	}
	bool fromstring(string val) override {
		try {
      setval(FormStr()(val));
      //		using boost::lexical_cast;
			//就是将string类型转换为模板T
    //    return		m_val = lexical_cast<T>(val);//将m_val流式处理返回到标准库中字符串的流中，
			//然后作为模板对象输出，这里模板对象就是T（就是将m_val转换为val，存储在string 然后以T形式返回）
		}
		catch (exception& e) {
			/*
			*e.what() 将异常信息打印出来
			*typeid().name()，可以用来返回变量的数据类型
			*
			*/
			LOG_ERROR(LOG_ROOT()) << "ConfigVal::ToString expection " << e.what() << " convert: string to" << typeid(m_val).name();
		}


	}
	T getval() { 
    RWM::readmutex lock(m_mutex);
    return m_val;
  }
	void setval(T val) { 
    {
     RWM::readmutex lock(m_mutex);
     if(val==m_val){
       return;
      }
     //回调
      for(auto &o:m_ck){
        o.second(m_val,val);
      }
    }
    RWM::writemutex lock(m_mutex);
    m_val = val;
  }
  string getTypeName(){return typeid(T).name();}
  //增加监听
  uint64_t AddListener(on_chang_ck ck){
    static uint64_t s_id=0;
    RWM::writemutex lock(m_mutex);
    ++s_id;
    m_ck[s_id]=ck;
    return s_id;
  }
  //删除
  void DelListener(uint64_t key){
    RWM::writemutex lock(m_mutex);
    m_ck.erase(key);
  }
  //返回配置项
 on_chang_ck GetChangCk(uint64_t key){
    RWM::writemutex lock(m_mutex);
    auto  it=m_ck.find(key);
    return it==m_ck.end()?NULL:it->second;
  }
 void ClearListener(){
    RWM::writemutex lock(m_mutex);
   m_ck.clear();
 }
private:
	T m_val;
  //变更函数组，回调函数没有比较函数，map不经能够通过key比较，而且还可以通过kry值直接删除配置选项
  //uint64_t  key 要求唯一   一般可以使用hash值
 map<uint64_t,on_chang_ck> m_ck;
 RWM m_mutex;
};


//配置管理器

class Config {
  public:
  typedef rwMutex RWM;
	typedef map<string, ConfigVarBase::ptr> ConfigValMap;
    static ConfigValMap &GetDatas(){
      static ConfigValMap s_datas;
      return s_datas;
    }
    static RWM& GetRwm(){
      static RWM s_mutex;
      return s_mutex;
    }
public:

	template<class T>
	static   typename ConfigVar<T>::ptr LookUp(string name, T val, string description = "") {
     RWM::writemutex lock(GetRwm());
    //处理相同的值
    //防止map中出现相同的是值直接返回
		auto tmp = GetDatas().find(name);
    if(tmp!=GetDatas().end()){
      auto cv=dynamic_pointer_cast<ConfigVar<T>>(tmp->second);
  		if (cv) {
	  		LOG_INFO(LOG_ROOT()) << "LookUp name " << name << " exists";
		  	return cv;
      }else{
	  		LOG_ERROR(LOG_ROOT()) << "LookUp name " << name << " exists but type not "
          <<typeid(T).name()<<"type name="<<tmp->second->getTypeName()<<" "<<tmp->second->Tostring();
        return NULL;
      }
    }
		if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._12345678") != string::npos) {
			//正向查找在原字符串中第一个与指定字符串（或字符）中的任一字符都不匹配的字符，返回它的位置。若查找失败，则返回npos。
			LOG_ERROR(LOG_ROOT()) << "LookUp name invalid " << name;
				throw invalid_argument(name);//抛出无效参数name
		}
		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, val, description));
		GetDatas()[name] = v;
		return v;

	}
	template<class T>
	static typename   ConfigVar<T>::ptr LookUp(string name) {
     RWM::readmutex lock(GetRwm());
		auto it = GetDatas().find(name);
		//没有找到
		if (it == GetDatas().end()) {
			return nullptr;
		}
		//智能指针的向上转换  派生类-->基类
		//没有智能指针时可用dynamic_cast转换
		return dynamic_pointer_cast<ConfigVar<T>> (it->second);

	}
  //查找当前有没有命名的项
  //有就直接返回
//找之前的约定的配置项
 static  ConfigVarBase::ptr LookUpBase(string name){
     RWM::readmutex lock(GetRwm());
  auto it=  GetDatas().find(name);  
  return it==GetDatas().end()?NULL:it->second;  
}
  //将所有的node打印出来
 static void ListAllMeber(string prefix,const YAML::Node node,list<pair<string,YAML::Node>> &output){
    //校验名称是否合法

		if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._12345678") != string::npos) {
			LOG_ERROR(LOG_ROOT()) << "Config name invalid " << prefix<<":"<<node;
				return;
   }
    output.push_back(make_pair(prefix,node));
    if(node.IsMap()){
      for(auto it=node.begin();it!=node.end();it++){
          ListAllMeber(prefix.empty()?it->first.Scalar():prefix+"."+it->first.Scalar(),it->second,output);
        
      }
    }
  }
  
   static void LoadFormYaml(YAML::Node root){
     //输出型参数
    list<pair<string,YAML::Node>>all_node;
    //设置node
    ListAllMeber("",root,all_node);
    //遍历所有的结构看是否和一致的有一样的
    for(auto i:all_node){
      string key=i.first;
      if(key.empty()){
        continue;
      }
      //大小写转换
      transform(key.begin(),key.end(),key.begin(),::tolower);
      ConfigVarBase::ptr val=LookUpBase(key);
      if(val){
        if(i.second.IsScalar()){
          val->fromstring(i.second.Scalar());
        }else {
          stringstream ss;
          ss<<i.second;
          val->fromstring(ss.str());  
        }
      }
    }
  }
  static void Visit(function<void (ConfigVarBase::ptr)>cb){
    RWM::readmutex lock(GetRwm());
     ConfigValMap &m=GetDatas();
     for(auto it=m.begin();it!=m.end();it++){
       cb(it->second);
     }
    
    
  }
};
#endif // !__CONIFIG_H__
