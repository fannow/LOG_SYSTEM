#ifndef _ADDLC_HPP_
#define _ADDLC_HPP_
#include"Log.hpp"
#include"Config.h"


//配置系统和日志系统整合
struct LogAppenderDefine {
    int type = 0;//日志输出地  1  输出到文件 2 输出到控制台
    LogLevel::Level level = LogLevel::UNKNOW;
    string formater;//日志输出格式
    string file;
    bool operator==(const LogAppenderDefine& l)const {

        return type == l.type && level == l.level &&
            formater == l.formater && file == l.file;


    }
};


struct LogDefine {
    string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    string formater;


    vector<LogAppenderDefine> addernders;

    bool operator==(const LogDefine& l)const {
        return name == l.name && level == l.level && formater == l.formater && addernders == l.addernders;
    }
    bool operator <(const LogDefine& l) const{
        return  name < l.name;

    }
};
//日志系统的偏特化
template<>
class LexicalCast<string,set<LogDefine>>{
  public:
   set<LogDefine> operator()(string str){
     stringstream ss;
      YAML::Node root=YAML::Load(str);//这里转换为数组
     typename   std::set<LogDefine> vec;
      for(int i=0;i<root.size();i++){
          auto it=root[i];
          if(!it["name"].IsDefined()){
            cout<<"log config error: name is null"<<it<<endl;
            continue;
          }
        LogDefine id;
        id.name=it["name"].as<string>();
        id.level=LogLevel::Formstring(it["level"].IsDefined()?it["level"].as<string>(): "");
        if(it["formater"].IsDefined()){
          id.formater=it["formater"].as<string>();

        }
        if(it["addernders"].IsDefined()){
          for(int x=0;x<it["addernders"].size();x++){
            auto a=it["addernders"][x];
            LogAppenderDefine lad;
            if(!a["type"].IsDefined()){
            cout<<"log config error: appender is null"<<a<<endl;
              continue;
            }
            string type =a["type"].as<string>();
            if(type=="FileAppender"){
                lad.type=1;
                //日志文件路径
                if(!a["file"].IsDefined()){
                          
                cout<<"log config error: file  is invaild"<<endl;
                continue;
                }
                lad.file=a["file"].as<string>();
              if(a["formater"].IsDefined()){
                lad.formater=a["formater"].as<string>();
               }
            }else if(type=="StdOutAppender"){
                lad.type=2;
            }else{
            cout<<"log config error: addernder type is invaild "<<type<<endl;
              continue;
            }
            id.addernders.push_back(lad);
          }       
        }
         vec.insert(id);

      }
      return vec;
    }
     
};
template<>
class LexicalCast<set<LogDefine>,string>{
  public:
   string operator()(set<LogDefine> &v){
     stringstream ss;
     YAML::Node root;
      for(auto &o:v){
      YAML::Node n;
      n["name"]=o.name;
      n["level"]=LogLevel::Tostring(o.level);
      if(!o.formater.empty()){
        n["formater"]=o.formater;
      }
      for(auto &i:o.addernders){
        YAML::Node a;
        if(i.type==1){
            a["type"]="FileAppender";
            a["file"]=i.file;
        }else if(i.type==2){
          a["type"]="StdOutAppender";
        }
        a["level"]=LogLevel::Tostring(i.level);   
         if(!i.formater.empty()){
          a["formater"]=i.formater;
         }
        n["addernders"].push_back(n);
       }
      root.push_back(n);
      }
      ss<<root;
      return ss.str();
   }
};
ConfigVar<set<LogDefine>>::ptr logdefine= Config::LookUp("logs", set<LogDefine>(), "logs config");
struct LogIniner {
    LogIniner() {
        logdefine->AddListener([](const set<LogDefine>& old, const set<LogDefine>& nwe) {
            //新增
            for (auto& o : nwe) {
                Logger::ptr logger;
                auto it = old.find(o);
                if (it == old.end()) {
                    //在老的中寻找如果没有找到就添加到进来
                    logger=LOG_NAME(o.name);            
                }
                else {
                    if (!(o == *it)) {
                        //修改logger
                        logger= LOG_NAME(o.name);//先找到对应logger
                    }
          
                }
                logger->setleve(o.level);
                if (!o.formater.empty()) {
                    logger->SetFomater(o.formater);     
                }

                logger->ClearAppender();
                for (auto& i : o.addernders) {
                    LogAppender::ptr ap;
                    if (i.type == 1) {
                        //添加文件输出
                        ap.reset(new FileAppender(i.file));
                    }
                    else if (i.type == 2) {
                        //添加控制台输出
                        ap.reset(new StdOutAppender());
                    }
                    //添加输出格
                    //式
        
                    if(!i.formater.empty()){
                      Formater::ptr fmt(new Formater(i.formater));
                      if(!fmt->IsError()){
                        ap->SetFormat(fmt);
                      }else{
                        cout<<"appender formater is inviled"<<endl;
                      }
                    }
                    ap->setlevel(i.level);
                    logger->AddAppender(ap);

            }
                        //删除  ------在新的中找不见老的就删除
                        for (auto& i : old) {
                            auto it = nwe.find(i);
                            if (it == nwe.end()) {
                                auto logger = LOG_NAME(i.name);
                                logger->setleve((LogLevel::Level)100);
                                logger->ClearAppender();
                          }
                      }

                  }
        });
      }
            
   };//日志相系统初始化
struct LogIniner _init_log;
#endif
