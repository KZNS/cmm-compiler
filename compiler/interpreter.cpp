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
  if(pos1 != s.length()){
    v.push_back(s.substr(pos1));
  }
    if(c==" "&&v.size()>0){
        vector<string> v2;
        string s2;
        v2.push_back(v[0]);
        if(v.size()>1){
            for(int i=1;i<v.size();i++){
                if(i!=1){
                    s2 += " ";
                }
                s2 += v[i];
            }
        }
        v2.push_back(s2);
        v = v2;
        return;
    }
}
void SplitStringWithSpace(const std::string& s, std::vector<std::string>& v){
    int pos = s.find_first_of(' ');
    v.push_back(s.substr(0,pos));
    if(!s.substr(pos+1,s.length()-1).empty()){
        v.push_back(s.substr(pos+1,s.length()-1));
    }
}
bool isnum(string s)  
{  
        stringstream sin(s);  
        double t;  
        char p;  
        if(!(sin >> t))  
        /*解释： 
            sin>>t表示把sin转换成double的变量（其实对于int和float型的都会接收），如果转换成功，则值为非0，如果转换不成功就返回为0 
        */  
               return false;  
        if(sin >> p)  
        /*解释：此部分用于检测错误输入中，数字加字符串的输入形式（例如：34.f），在上面的的部分（sin>>t）已经接收并转换了输入的数字部分，在stringstream中相应也会把那一部分给清除，如果此时传入字符串是数字加字符串的输入形式，则此部分可以识别并接收字符部分，例如上面所说的，接收的是.f这部分，所以条件成立，返回false;如果剩下的部分不是字符，那么则sin>>p就为0,则进行到下一步else里面 
          */  
                return false;  
        else  
                return true;  
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
    this->cmdHandler["arg"] = do_arg;
    this->cmdHandler["ret"] = do_ret;
}
//add / sub / mul / div / mod / cmpeq / cmpne / cmpgt 
// cmplt / cmpge / cmple / and / or / not / neg
int PcodeInterpreter::get_var(const string varName){//接受元素名/数组名[下标]，返回其在runtimeVar中的下标
    if(varName.find('[')==varName.npos){
        if(runtimeVarLookup.top().find(varName)==runtimeVarLookup.top().end()){
            logger.error("access to undefined variable %s",varName.c_str());
            exit(-1);
        }
        return runtimeVarLookup.top()[varName]; 
    }
    return -1;
}
void PcodeInterpreter::check_rtstack_size(const int n){
    if(runtimeStack.size()<n){
        logger.error("Runtime error: invalid stack depth = %d, wants %d",runtimeStack.size(),n);
        exit(-5);
    }
}
bool PcodeInterpreter::var_exists(const string varName){
    if(runtimeVarLookup.top().find(varName)==runtimeVarLookup.top().end())return false;
    return true;
}
int PcodeInterpreter::do_arg(const string cmd){
    unordered_map<string,int> m;
    runtimeVarLookup.push(m);
    if(cmd==""){
        return 0;
    }
    cout << "cmd size:"<<cmd.length()<<"|"<<cmd<<endl;
    vector<string> args;
    SplitString(cmd,args,",");
    check_rtstack_size(args.size());
    reverse(args.begin(),args.end());
    for(auto a:args){
        vector<string> attr;
        SplitString(a,attr,":");
        string dtype = attr.size()<2 ? "int" : attr[1];
        runtimeVarLookup.top()[attr[0]] = runtimeVar.size();
        runtimeVar.push_back({dtype,attr[0],runtimeStack.top()});//type,name,val
        cout << "arg:"<<a<<"="<<runtimeStack.top()<<endl;
        runtimeStack.pop();
    }
    return 0;
}
int PcodeInterpreter::do_ret(const string cmd){
    if(cmd=="~"){
        cout << "is~"<<endl;
        int t = runtimeStack.top();
        runtimeStack.pop();
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for(int i=n;i>=o;i--){
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        runtimeStack.push(t);
        //需要pop和push里记录用了几次，最后在这里运行时栈
        eip.pop();
        return 0;
    } else if(cmd=="") {
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
    } else if(var_exists(cmd)){
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
    check_rtstack_size(1);
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
        } else if(i.find('~')!=i.npos){//输出栈顶元素并pop
            check_rtstack_size(1);
            cout << runtimeStack.top();
            runtimeStack.pop();
            //这里后续判断类型
        } else {
            cout << runtimeVar[get_var(i)].val;
        }
    }
    cout << endl;
    return 0;
}
int PcodeInterpreter::do_exit(const string cmd){
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
    cout << "-rtvarlookupsize---"<<endl;
    cout << runtimeVarLookup.size() << endl;
    cout << "-eipsize----"<<endl;
    cout << eip.size() << endl;
    exit(0);
}
int PcodeInterpreter::do_input(const string cmd){
    //<r_stmt> ::= scanf'('<ident>{,<ident>}')'
    vector<string> vars;
    string msg;
    SplitString(cmd,vars,",");
    for(auto var:vars){
        cin >> runtimeVar[get_var(var)].val;
        logger.debug("setting %s to %s",runtimeVar[get_var(var)].name.c_str(),to_string(runtimeVar[get_var(var)].val).c_str());
    }
    cout << msg<<endl;
    return 0;
}
int PcodeInterpreter::do_mul(const string dummy){
    check_rtstack_size(2);
    int r = runtimeStack.top();
    runtimeStack.pop();
    r *= runtimeStack.top();
    runtimeStack.pop();
    runtimeStack.push(r);
    logger.info("multiplied to %d",r);
    return 0;
}
int PcodeInterpreter::do_add(const string dummy){
    check_rtstack_size(2);
    int r = runtimeStack.top();
    runtimeStack.pop();
    r += runtimeStack.top();
    runtimeStack.pop();
    runtimeStack.push(r);
    logger.info("added to %d",r);
    return 0;
}
int PcodeInterpreter::do_var(const string cmd){
    vector<string> vars;
    SplitString(cmd,vars,",");
    for(auto var:vars){
        vector<string> attr;
        SplitString(var,attr,":");
        if(var_exists(attr[0])){
            logger.error("duplicate definition of variable %s",var.c_str());
            exit(-1);
        }
        if(attr[0].find('[')!=attr[0].npos){//array mode
            int len = stoi(attr[0].substr(attr[0].find_first_of('[')+1,attr[0].find_first_of(']')-2));
            for(int i=0;i<len;i++){
                string vName = attr[0].substr(0,attr[0].find_first_of('['))+"["+to_string(i)+"]";
                runtimeVarLookup.top()[vName] = runtimeVar.size();
                runtimeVar.push_back({attr[1],attr[0],0});
                old_sp.top()++;
            }
            logger.info("declared array variable %s (%d)",attr[0].substr(0,attr[0].find_first_of('[')).c_str(),len);
        } else {
            runtimeVarLookup.top()[attr[0]] = runtimeVar.size();
            runtimeVar.push_back({attr[1],attr[0],0});
            old_sp.top()++;
            logger.info("declared variable %s",attr[0].c_str());
        }
    }
    return 0;
}
int PcodeInterpreter::do_push(const string cmd){//如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    int a;
    string msg;
    if(var_exists(cmd)){
        a = runtimeVar[get_var(cmd)].val;
        msg += "variable:";msg+=cmd; 
    } else {
        if(!isnum(cmd)){
            logger.fatal("invalid push argument %s",cmd);
            exit(-3);
        }
        stringstream ss;
        ss<<cmd;
        ss>>a;
    }
    runtimeStack.push(a);
    logger.info("push: %d %s",a,msg.c_str());
    return 0;
}
int PcodeInterpreter::do_pop(const string cmd){//如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    if(cmd==""){//直接pop，不管了
        runtimeStack.pop();
        cout <<" direct pop"<<endl;
        return 0;
    }
    runtimeVar[get_var(cmd)].val = runtimeStack.top();
    runtimeStack.pop();
    logger.info("pop into %s",cmd.c_str());
    return -1;
}
int PcodeInterpreter::func_call(const string funcName){
    string fname = funcName.substr(1,funcName.length()-1);
    if(funcMap.find(fname)!=funcMap.end()){
        logger.info("eip pushed %d",funcMap[fname]);
        eip.push(funcMap[fname]);
        old_sp.push(old_sp.top()+1);
    } else {
        cout << fname<<" is not a valid funcname"<<endl;
        return -1;
    }
    return 0;
}
int PcodeInterpreter::interpret(const std::string &in_file_name){
    ifstream myfile(in_file_name);
    string temp;
    if(!myfile.is_open()){
        cout << "failed to open file" << endl;
        return -1;
    }
    while(getline(myfile,temp)){
        temp = trim(temp);

        if(temp[0]==';')continue;
        if(temp.empty())continue;
        //cout << temp << endl;
        vector<string> v;
        SplitString(temp,v," ");//这里不该这样写，应该只用空格分一次，需要重写split
        //SplitStringWithSpace(temp,v);
        this->code.push_back(v);
        if(v.size()==1 && v[0].back()==':'){
            string labelName = v[0].substr(0,v[0].size()-1);
            if(labelMap.find(labelName)!=labelMap.end()){
                printf("duplicate labels error.");
                return -1;
            }
            this->labelMap.insert(make_pair(labelName,this->code.size()));
        } else if(v.size()==2 && v[0]=="FUNC" && v[1].back()==':'){
            string funcName = v[1].substr(1,v[1].size()-2);
            if(funcMap.find(funcName)!=funcMap.end()){
                printf("duplicate funcname error");
                return -1;
            }
            this->funcMap.insert(make_pair(funcName,this->code.size()-1));//回退到函数入口
        }
    }
    myfile.close();
    int i=0;
    while(i<99){
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
            logger.warn("[!]second command argument is empty!");
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
    return 0;
}
#endif

//数组第二个是下标，第一个是值
//pop 第一个是下标