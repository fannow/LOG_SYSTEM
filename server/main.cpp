#include"addlc.hpp"
#include"thread.h"
#include<yaml-cpp/yaml.h>
/*	
ConfigVar<int>::ptr config_in = Config::LookUp("system.port", (int)8080, "system port");
ConfigVar<float>::ptr configx_in = Config::LookUp("system.port", (float)8080, "system port");
//ConfigVar<int>::ptr config_in = Config::LookUp("system.port", (int)8080, "system port");
ConfigVar<float>::ptr config_out= Config::LookUp("system.value", (float)8080, "system value");
ConfigVar<vector<int>>::ptr config_vec= Config::LookUp("system.in_vec",vector<int>{1,2}, "system in_vec");
ConfigVar<set<int>>::ptr config_set= Config::LookUp("system.in_set",set<int>{1,2}, "system in_set");
ConfigVar<unordered_set<int>>::ptr config_unset= Config::LookUp("system.in_unset",unordered_set<int>{1,2}, "system in_unset");
ConfigVar<map<string,int>>::ptr config_map= Config::LookUp("system.in_map",map<string,int>{{"k",1},{"d",2}}, "system in_map");
//ConfigVar<list<int>>::ptr config_list= Config::LookUp("system.in_list",list<int>{1,2}, "system in_list");
void print(YAML::Node node,int level){
  if(node.IsScalar()){
    //yaml-------简单结构-----------直接输出
    LOG_INFO(LOG_ROOT())<<string(level*4,' ')<<node.Scalar()<<" - "<<node.Type()<<" - "<<level;
  }else if(node.IsNull()){

    LOG_INFO(LOG_ROOT())<<string(level*4,' ')<<"NULL - "<<node.Type()<<" - "<<level;
  }else if(node.IsMap()){
    //yaml------map类型-------------遍历
    for(auto it=node.begin();it!=node.end();it++){

     LOG_INFO(LOG_ROOT())<<string(level*4,' ')<<it->first<<" - " <<it->second.Type()<<" - "<<level;
     print(it->second,level+1);
    }
  }else if(node.IsSequence()){
    //yaml------线性结构-----------遍历
    for(size_t i=0;i<node.size();i++){

    LOG_INFO(LOG_ROOT())<<string(level*4,' ')<<i<<" - "<<node[i].Type()<<" - "<<level;
    print(node[i],level+1);
    }

  }

}
void test_yaml(){
  //加载yaml文件
  
	YAML::Node root=YAML::LoadFile("./test.yml");
  print(root,0);
}
void test_config(){

  LOG_INFO(LOG_ROOT())  <<"config_in"<<config_in->getval();
  LOG_INFO(LOG_ROOT())  <<"configx_in"<<configx_in->getval();
 // LOG_INFO(LOG_ROOT())  <<"in_set "<<i;
  LOG_INFO(LOG_ROOT())  <<"config_out"<<config_out->getval();
// auto m=config_map->getval;
// for(auto it=m.begin();it!=m.end();it++){
//
//  LOG_INFO(LOG_ROOT())  <<"in_map "<<it->first<<"-"<<it->second;
// }
  auto a=config_unset->getval(); 
  for(auto&i:a){
  LOG_INFO(LOG_ROOT())  <<"in_set "<<i;
  }
 // auto v=config_list->getval();
  //for(auto&i:v){
  //LOG_INFO(LOG_ROOT())  <<"in_list "<<i;
 // }
	YAML::Node root=YAML::LoadFile("./test.yml");
  Config::LoadFormYaml(root);
//  LOG_INFO(LOG_ROOT())  <<"config_out"<<config_out->getval();
 // for(auto&i:v){
 // LOG_INFO(LOG_ROOT())  <<"in_list "<<i;
 // }
 a=config_unset->getval();
  for(auto&i:a){
  LOG_INFO(LOG_ROOT())  <<"in_set "<<i;
}}
*/
/*
ConfigVar<person>::ptr config_person = Config::LookUp("class.person", person(), "class person");
//ConfigVar<map<string,vector<person>>>::ptr config_map= Config::LookUp("class.in_map",map<string,vector<person>>(), "class person");
//ConfigVar<map<string,person>>::ptr config_map= Config::LookUp("system.port",map<string,person>{{"k",person()},{"b",person()},{"c",person()}}, "system in_map");
//ConfigVar<map<string,int>>::ptr config_map1= Config::LookUp("system.in_map",map<string,int>{{"k",1},{"d",2}}, "system in_map");
//ConfigVar<vector<map<string,int>>>::ptr config_map1= Config::LookUp("system.in_map",vector<map<string,int>>{{{"k",1},{"d",2}},{{"c",person()},{"e",person()}}}, "system in_map");
//自定义类型测试
void test_class(){
  LOG_INFO(LOG_ROOT())  <<config_person->getval().tostring();
 // LOG_INFO(LOG_ROOT())  <<"config map vector  "<<m;
  config_person->AddListener(10,[]( const person &old,const person &new1){         
  LOG_INFO(LOG_ROOT())  <<"old ="<<old.tostring()<<"new ="<<new1.tostring();
        });
	YAML::Node root=YAML::LoadFile("./test.yml");
  Config::LoadFormYaml(root);
  config_person->AddListener(10,[]( const person &old,const person &new1){         
  LOG_INFO(LOG_ROOT())  <<"old ="<<old.tostring()<<"new ="<<new1.tostring();
        });
 //auto mq=config_map->Tostring();
//  LOG_INFO(LOG_ROOT())  <<"config map vector  "<<mq;
  LOG_INFO(LOG_ROOT())  <<"config person "<<config_person->getval().tostring();

// auto m=config_map1->getval();
// for(auto it=m.begin();it!=m.end();it++){
//
// LOG_INFO(LOG_ROOT())  <<"in_map "<<it->first<<"-"<<it->second;
// }
//
 //auto m=config_map1->getval();
 //for(auto it=m.begin();it!=m.end();it++){
 // LOG_INFO(LOG_ROOT())  <<"in_map "<<it->first<<"-"<<it->second;
 /
 }
}*/

void log(){
  Logger::ptr logge1r=LOG_NAME("system");
  LOG_INFO(logge1r)<<"hello system"<<endl;
  cout<<LogRoot::GetInstance()->toyamlstring()<<endl;
	YAML::Node root=YAML::LoadFile("./test.yml");
  Config::LoadFormYaml(root);
  cout<<LogRoot::GetInstance()->toyamlstring()<<endl;
  Config::Visit([](ConfigVarBase::ptr var){
      LOG_INFO(LOG_ROOT())<<" name="<<var->getname()
      <<" des="<<var->getdescription()
      <<" ty="<<var->getTypeName()
      <<" val="<<var->Tostring();
      });
}

Mutex mu;
//rwMutex s_mutex;
int coun=0;
Logger::ptr logg=LOG_NAME("system");
void fun1(){
  
  LOG_INFO(logg)<<"thread name="<<Thread::GetName()<<" this.name="
    <<Thread::getThis()->getName()<<" pid="<<Thread::getThis()->getId()<<" until id="<<GetThreadId();
  for(int i=0;i<100000000;i++){
    Mutex::Lock Lock(mu);
    coun++;
  }
}
void fun2(){

  LOG_INFO(logg)<<"******************************";

}
void fun3(){
  LOG_INFO(logg)<<"_____________________________";
}
void test_thread(){
  LOG_INFO(logg)<<"thread is testing";
  vector<Thread::ptr> th;
  for(int i=0;i<2;i++){
    Thread::ptr thread(new Thread(&fun2,to_string(i)));
    Thread::ptr thread1(new Thread(&fun3,to_string(i)));
    th.push_back(thread);
    th.push_back(thread1);
  }
   for(int i=0;i<th.size();i++){
     th[i]->join();
    }
  LOG_INFO(logg)<<"thread is end";
  LOG_INFO(logg)<<"count:"<<coun;
  Config::Visit([](ConfigVarBase::ptr var){
      LOG_INFO(LOG_ROOT())<<"name="<<var->getname()
      <<"des="<<var->getdescription()
      <<"ty="<<var->getTypeName()
      <<"val"<<var->Tostring();
      });
}
#include"macro.h"
#include"fiber.h"
void run_fiber(){

  LOG_INFO(LOG_ROOT())<<"run_in_fiber begin";
  Fiber::YieIdToHold();
  LOG_INFO(LOG_ROOT())<<"run_in_fiber end";
  Fiber::YieIdToHold();
}
int main() {
  LOG_INFO(LOG_ROOT())<<"main begin";
  {
  Fiber::GetThis();
  Fiber::ptr fiber(new Fiber(run_fiber));
  fiber->SwapIn();
  LOG_INFO(LOG_ROOT())<<"main swapIn";
  fiber->SwapIn();
  LOG_INFO(LOG_ROOT())<<"main end";
  fiber->SwapIn();
  }
  //ASSERT2(false,"123");
  //cout<<BacktraceTostring(20,0,"   ");

return 0;
}
