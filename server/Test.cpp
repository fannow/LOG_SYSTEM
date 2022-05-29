#include"addlc.hpp"
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
 // }
}*/
void log(){
  cout<<LogRoot::GetInstance()->toyamlstring()<<endl;
	YAML::Node root=YAML::LoadFile("./test.yml");
  Config::LoadFormYaml(root);
  cout<<LogRoot::GetInstance()->toyamlstring()<<endl;

}
int main() {
  log();

return 0;
}
