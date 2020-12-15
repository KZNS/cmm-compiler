#ifndef INTERPRETER_CPP
#define INTERPRETER_CPP
#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}
string& trim(string &s) 
{
    if (s.empty()) 
    {
        return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
PcodeInterpreter::PcodeInterpreter(){
    unordered_map<string,int> m;
    this->eip.push(0);
    this->old_sp.push(0);
    this->runtimeVarLookup.push(m);
    this->cmdHandler["var"] = do_var;
    this->cmdHandler["push"] = do_push;
    this->cmdHandler["pop"] = do_pop;
    this->cmdHandler["add"] = do_add;
    this->cmdHandler["mul"] = do_mul;
    this->cmdHandler["print"] = do_print;
    this->cmdHandler["input"] = do_input;
    this->cmdHandler["jmp"] = do_jmp;
    this->cmdHandler["jz"] = do_jz;
    this->cmdHandler["exit"] = do_exit;
}
//add / sub / mul / div / mod / cmpeq / cmpne / cmpgt 
// cmplt / cmpge / cmple / and / or / not / neg
int PcodeInterpreter::do_arg(const string cmd){
    vector<string> args;
    unordered_map<string,int> m;
    runtimeVarLookup.push(m);
    SplitString(cmd,args,",");
    if(runtimeStack.size()<args.size()){
        cout << "invalid stack depth in do_Args"<<endl;
        return -1;
    }
    reverse(args.begin(),args.end());
    for(auto a:args){
        runtimeVarLookup.top()[a] = runtimeVar.size();
        runtimeVar.push_back({"arg",a,runtimeStack.top()});//type,name,val
        runtimeStack.pop();
    }
    return 0;
}
int PcodeInterpreter::do_ret(const string cmd){
    if(cmd=="~"){
        int t = runtimeStack.top();
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for(int i=n;i>o;i--){
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        runtimeStack.push(t);
        //需要pop和push里记录用了几次，最后在这里运行时栈
        eip.pop();
        return 0;
    } else if(cmd==""){
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for(int i=n;i>o;i--){
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        //pop everything in this func and return
        eip.pop();
        return 0;
    } else if(runtimeVarLookup.top().find(cmd)!=runtimeVarLookup.top().end()){
        //for(int i=)
        int t = runtimeVar[runtimeVarLookup.top()[cmd]].val;
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for(int i=n;i>o;i--){
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        runtimeStack.push(t);
        eip.pop();
        return 0;
    }
    return 0;
}
int PcodeInterpreter::do_jmp(const string cmd){
    if(labelMap.find(cmd)!=labelMap.end()){
        int tar = labelMap[cmd];
        cout << "jmp target:"<<tar<<endl;
        //cout << code[tar][0]<<endl;
        eip.top() = tar-1;
    } else {
        cout << cmd << " not found in label map."<<endl;
        return -1;
    }
    return 0;
}
int PcodeInterpreter::do_jz(const string cmd){
    if(runtimeStack.size()<1){
        cout << "invalid runtime stack depth"<<endl;
        return -1;
    }
    if(labelMap.find(cmd)==labelMap.end()){
        cout << cmd << " not found in label map."<<endl;
        return -1;
    }
    int z = runtimeStack.top();
    runtimeStack.pop();
    if(z==0){
        int tar = labelMap[cmd];
        eip.top() = tar-1;
    } else {
        return 0;
    }
    return 0;
}
int PcodeInterpreter::do_print(const string cmd){
    vector<string> info;
    SplitString(cmd,info,",");
    for(auto i:info){
        if(i.front()=='\"'&&i.back()=='\"'){
            cout << i.substr(1,i.length()-2);
        } else if(runtimeVarLookup.top().find(i)!=runtimeVarLookup.top().end()){
            cout << runtimeVar[runtimeVarLookup.top()[i]].val;
        }
    }
    cout << endl;
    return 0;
}
int PcodeInterpreter::do_exit(const string cmd){
    exit(0);
}
int PcodeInterpreter::do_input(const string cmd){
    //<r_stmt> ::= scanf'('<ident>{,<ident>}')'
    vector<string> vars;
    string msg;
    SplitString(cmd,vars,",");
    for(auto v:vars){
        if(runtimeVarLookup.top().find(v)!=runtimeVarLookup.top().end()){
            for(int i=old_sp.top();i<runtimeVar.size();i++){
                if(runtimeVar[i].name==v){//暂时不考虑变量未初始化的问题，默认是0
                    cin >> runtimeVar[i].val;
                    msg += "setting "+runtimeVar[i].name+" to "+to_string(runtimeVar[i].val);
                }   
            }
        } else {
            cout << "runtime error: "<<v<<" is not defined"<<endl;
            return -1;
        }
    }
    cout << msg<<endl;
    return 0;
}
int PcodeInterpreter::do_mul(const string dummy){
    if(runtimeStack.size()<2){
        cout << "invalid stack depth"<<endl;
        return -1;
    }
    int r = runtimeStack.top();
    runtimeStack.pop();
    r *= runtimeStack.top();
    runtimeStack.pop();
    runtimeStack.push(r);
    cout <<"multiplied to "<<r << endl;
    return 0;
}
int PcodeInterpreter::do_add(const string dummy){
    if(runtimeStack.size()<2){
        cout << "invalid stack depth"<<endl;
        return -1;
    }
    int r = runtimeStack.top();
    runtimeStack.pop();
    r += runtimeStack.top();
    runtimeStack.pop();
    runtimeStack.push(r);
    cout <<"added to "<<r << endl;
    return 0;
}
int PcodeInterpreter::do_var(const string cmd){
    vector<string> vars;
    SplitString(cmd,vars,",");
    for(auto var:vars){
        vector<string> attr;
        SplitString(var,attr,":");
        if(runtimeVarLookup.top().find(attr[0])!=runtimeVarLookup.top().end()){
            cout << "duplicate definition of variable "<<var<<endl;
            return -1;
        } else {
            runtimeVarLookup.top()[attr[0]] = runtimeVar.size();
            runtimeVar.push_back({attr[1],attr[0],0});
            old_sp.top()++;
        }
    }
    cout << "cmd:"<<cmd << endl;
    return 0;
}
int PcodeInterpreter::do_push(const string cmd){//如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    int a;
    if(runtimeVarLookup.top().find(cmd)!=runtimeVarLookup.top().end()){//这是个变量名，去找变量值
        for(int i=old_sp.top();i<runtimeVar.size();i++){
            if(runtimeVar[i].name==cmd){//暂时不考虑变量未初始化的问题，默认是0
                a = runtimeVar[i].val;
                cout << "variable detected:";
            }
            break;
        }
    } else {
        stringstream ss;
        ss<<cmd;
        ss>>a;
    }
    runtimeStack.push(a);
    cout << "push:"<<a << endl;
    return 0;
}
int PcodeInterpreter::do_pop(const string cmd){//如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    cout << "pop:"<<cmd << endl;
    if(cmd==""){//直接pop，不管了
        runtimeStack.pop();
        cout <<" direct pop"<<endl;
        return 0;
    }
    if(runtimeVarLookup.top().find(cmd)!=runtimeVarLookup.top().end()){//这是个变量名，去找变量值
        for(int i=old_sp.top();i<runtimeVar.size();i++){
            if(runtimeVar[i].name==cmd){
                cout <<"indirect pop"<<endl;
                runtimeVar[i].val = runtimeStack.top();
                runtimeStack.pop();
                return 0;
            }
        }
    }
    return -1;
}
int PcodeInterpreter::func_call(const string funcName){
    string fname = funcName.substr(1,funcName.length());
    if(funcMap.find(fname)!=funcMap.end()){
        cout << fname<<" is not a valid funcname"<<endl;
        return -1;
    } else {
        eip.push(funcMap[fname]);
        old_sp.push(old_sp.top()+1);
    }
    return 0;
}
int PcodeInterpreter::interpret(const std::string &in_file_name){
    ifstream myfile(in_file_name);
    string temp;
    if(!myfile.is_open()){
        cout << "fail" << endl;
        return -1;
    }
    while(getline(myfile,temp)){
        temp = trim(temp);

        if(temp[0]==';')continue;
        if(temp.empty())continue;
        //cout << temp << endl;
        vector<string> v;
        SplitString(temp,v," ");//这里不该这样写，应该只用空格分一次，需要重写split
        this->code.push_back(v);
        if(v.size()==1 && v[0].back()==':'){
            string labelName = v[0].substr(0,v[0].size()-1);
            if(labelMap.find(labelName)!=labelMap.end()){
                printf("duplicate labels error.");
                return -1;
            }
            this->labelMap.insert(make_pair(labelName,this->code.size()));
        } else if(v.size()==2 && v[0]=="FUNC" && v[1].back()==':'){
            string funcName = v[1].substr(1,v[1].size()-1);
            if(funcMap.find(funcName)!=funcMap.end()){
                printf("duplicate funcname error");
                return -1;
            }
            this->funcMap.insert(make_pair(funcName,this->code.size()));//回退到函数入口
        }
    }
    int i=0;
    while(i<12){
        vector<string> cmd = code[eip.top()];
        if(cmd.size()>0){
            if(cmd[0].back()==':'){
                i++;eip.top()++;
                continue;
            } else if(cmd[0].front()=='$'){
                func_call(cmd[0]);
                i++;eip.top()++;
                continue;
            }
        }
        if(cmd.size()==2){
            (this->*cmdHandler[cmd[0]])(cmd[1]);
        } else {
            (this->*cmdHandler[cmd[0]])("");
        }
        //cout << "-->"<<cmd[0]<<endl;
        //int (PcodeInterpreter::*p)(const string);
        //p = this->cmdHandler[cmd[0]];
        //(this->*p)(cmd[1]);
        //break;
        i++;
        eip.top()++;
    }
    cout << "-labelMap---"<<endl;
    for(auto iter=labelMap.begin();iter!=labelMap.end();iter++){
        cout << iter->first << " : " << iter->second << endl;
    }
    cout << "-funcMap----"<<endl;
    for(auto iter=funcMap.begin();iter!=funcMap.end();iter++){
        cout << iter->first << " : " << iter->second << endl;
    }
    cout << "-variables--"<<endl;
    for(auto i:this->runtimeVar){
        cout << i.name << "("<<i.type<<")"<<"="<<i.val<<endl;
    }
    cout << "-codeSize---"<<endl;
    cout << this->code.size() << endl;
    myfile.close();
    return 0;
}
#endif

