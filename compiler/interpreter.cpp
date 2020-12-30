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
void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
    {
        v.push_back(s.substr(pos1));
    }
    if (c == " " && v.size() > 0)
    {
        vector<string> v2;
        string s2;
        v2.push_back(v[0]);
        if (v.size() > 1)
        {
            for (int i = 1; i < (int)v.size(); i++)
            {
                if (i != 1)
                {
                    s2 += " ";
                }
                s2 += v[i];
            }
        }
        if (v.size() > 1)
        {
            v2.push_back(s2);
        }
        v = v2;
        return;
    }
}
void SplitStringWithSpace(const std::string &s, std::vector<std::string> &v)
{
    int pos = s.find_first_of(' ');
    v.push_back(s.substr(0, pos));
    if (!s.substr(pos + 1, s.length() - 1).empty())
    {
        v.push_back(s.substr(pos + 1, s.length() - 1));
    }
}
bool isnum(string s)
{
    stringstream sin(s);
    double t;
    char p;
    if (!(sin >> t))
        /*解释： 
            sin>>t表示把sin转换成double的变量（其实对于int和float型的都会接收），如果转换成功，则值为非0，如果转换不成功就返回为0 
        */
        return false;
    if (sin >> p)
        /*解释：此部分用于检测错误输入中，数字加字符串的输入形式（例如：34.f），在上面的的部分（sin>>t）已经接收并转换了输入的数字部分，在stringstream中相应也会把那一部分给清除，如果此时传入字符串是数字加字符串的输入形式，则此部分可以识别并接收字符部分，例如上面所说的，接收的是.f这部分，所以条件成立，返回false;如果剩下的部分不是字符，那么则sin>>p就为0,则进行到下一步else里面 
          */
        return false;
    else
        return true;
}
string &trim(string &s)
{
    if (s.empty())
    {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
PcodeInterpreter::PcodeInterpreter()
{
    unordered_map<string, int> m;
    this->out = &cout;
    this->in = &cin;
    this->eip.push(0);
    this->old_sp.push(0);
    this->runtimeVarLookup.push(m);
    this->cmdHandler["var"] = &PcodeInterpreter::do_var;
    this->cmdHandler["push"] = &PcodeInterpreter::do_push;
    this->cmdHandler["pop"] = &PcodeInterpreter::do_pop;
    this->cmdHandler["add"] = &PcodeInterpreter::do_add;
    this->cmdHandler["mul"] = &PcodeInterpreter::do_mul;
    this->cmdHandler["print"] = &PcodeInterpreter::do_print;
    this->cmdHandler["input"] = &PcodeInterpreter::do_input;
    this->cmdHandler["jmp"] = &PcodeInterpreter::do_jmp;
    this->cmdHandler["jz"] = &PcodeInterpreter::do_jz;
    this->cmdHandler["jnz"] = &PcodeInterpreter::do_jnz;
    this->cmdHandler["exit"] = &PcodeInterpreter::do_exit;
    this->cmdHandler["arg"] = &PcodeInterpreter::do_arg;
    this->cmdHandler["ret"] = &PcodeInterpreter::do_ret;
    this->cmdHandler["sub"] = &PcodeInterpreter::do_sub;
    this->cmdHandler["div"] = &PcodeInterpreter::do_div;
    this->cmdHandler["mod"] = &PcodeInterpreter::do_mod;
    this->cmdHandler["cmpeq"] = &PcodeInterpreter::do_cmpeq;
    this->cmdHandler["cmpne"] = &PcodeInterpreter::do_cmpne;
    this->cmdHandler["cmpgt"] = &PcodeInterpreter::do_cmpgt;
    this->cmdHandler["cmplt"] = &PcodeInterpreter::do_cmplt;
    this->cmdHandler["cmpge"] = &PcodeInterpreter::do_cmpge;
    this->cmdHandler["cmple"] = &PcodeInterpreter::do_cmple;
    this->cmdHandler["and"] = &PcodeInterpreter::do_and;
    this->cmdHandler["or"] = &PcodeInterpreter::do_or;
    this->cmdHandler["not"] = &PcodeInterpreter::do_not;
    this->cmdHandler["neg"] = &PcodeInterpreter::do_neg;
}
int PcodeInterpreter::do_mul(const string dummy)
{
    int op_2 = get_rtstack_var();
    int op_1 = get_rtstack_var();
    int result = op_1 * op_2;
    runtimeStack.push(result);
    logger.info("multiplied to %d", result);
    return 0;
}
int PcodeInterpreter::do_add(const string dummy)
{ //op_1 + op_2
    int op_2 = get_rtstack_var();
    int op_1 = get_rtstack_var();
    int result = op_1 + op_2;
    runtimeStack.push(result);
    logger.info("added to %d", result);
    return 0;
}
int PcodeInterpreter::do_sub(const string dummy)
{ //op_1-op_2
    int op_1 = get_rtstack_var();
    int op_2 = get_rtstack_var();
    int result = op_2 - op_1;
    runtimeStack.push(result);
    logger.info("subtracted to %d", result);
    return 0;
}
int PcodeInterpreter::do_div(const string dummy)
{ //op_1 / op_2
    int op_2 = get_rtstack_var();
    int op_1 = get_rtstack_var();
    if (op_2 == 0)
    {
        logger.error("[%d] Runtime error: division by zero", eip.top());
        exit(-5);
    }
    int result = op_1 / op_2;
    runtimeStack.push(result);
    logger.info("divided to %d", result);
    return 0;
}
int PcodeInterpreter::do_mod(const string dummy)
{
    int op_2 = get_rtstack_var();
    int op_1 = get_rtstack_var();
    if (op_2 == 0)
    {
        logger.error("[%d] Runtime error: modular by zero", eip.top());
        exit(-5);
    }
    int result = op_1 % op_2;
    runtimeStack.push(result);
    logger.info("mod to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmpeq(const string dummy)
{
    int result = get_rtstack_var() == get_rtstack_var();
    runtimeStack.push(result);
    logger.info("cmp == to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmpne(const string cmd)
{
    int result = get_rtstack_var() != get_rtstack_var();
    runtimeStack.push(result);
    logger.info("cmp != to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmpgt(const string cmd)
{
    int a, b;
    b = get_rtstack_var();
    a = get_rtstack_var();
    int result = a > b;
    runtimeStack.push(result);
    logger.info("cmp gt to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmplt(const string cmd)
{
    int a, b;
    b = get_rtstack_var();
    a = get_rtstack_var();
    int result = a < b;
    runtimeStack.push(result);
    logger.info("cmp lt to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmpge(const string cmd)
{
    int a, b;
    b = get_rtstack_var();
    a = get_rtstack_var();
    int result = a >= b;
    runtimeStack.push(result);
    logger.info("cmp >= to %d", result);
    return 0;
}
int PcodeInterpreter::do_cmple(const string cmd)
{
    int a, b;
    b = get_rtstack_var();
    a = get_rtstack_var();
    int result = (a <= b);
    runtimeStack.push(result);
    logger.info("cmp <= to %d", result);
    return 0;
}
int PcodeInterpreter::do_and(const string cmd)
{
    int result = get_rtstack_var() && get_rtstack_var();
    runtimeStack.push(result);
    logger.info("and-op to %d", result);
    return 0;
}
int PcodeInterpreter::do_or(const string cmd)
{
    int result = get_rtstack_var() || get_rtstack_var();
    runtimeStack.push(result);
    logger.info("or-op to %d", result);
    return 0;
}
int PcodeInterpreter::do_not(const string dummy)
{
    check_rtstack_size(1);
    runtimeStack.top() = !runtimeStack.top();
    logger.info("not-op to %d", runtimeStack.top());
    return 0;
}
int PcodeInterpreter::do_neg(const string dummy)
{
    check_rtstack_size(1);
    runtimeStack.top() = -runtimeStack.top();
    logger.info("neg-op to %d", runtimeStack.top());
    return 0;
}
int PcodeInterpreter::get_rtstack_var()
{
    check_rtstack_size(1);
    int r = runtimeStack.top();
    runtimeStack.pop();
    return r;
}
int PcodeInterpreter::get_var(const string varName)
{ //接受元素名/数组名[下标]，返回其在runtimeVar中的下标
    string vName;
    if (varName.find('[') == varName.npos)
    { //是变量
        vName = varName;
    }
    else
    { //是数组
        check_rtstack_size(1);
        int _index = runtimeStack.top();
        runtimeStack.pop();
        vName = varName.substr(0, varName.find_first_of('[') + 1) + to_string(_index) + varName.substr(varName.find_first_of(']'));
    }
    if (runtimeVarLookup.top().find(vName) == runtimeVarLookup.top().end() && runtimeGlobLookup.find(vName) == runtimeGlobLookup.end())
    {
        logger.error("[%d] Runtime error: access to undefined variable %s", eip.top(), vName.c_str());
        exit(-1);
    }
    if (runtimeVarLookup.top().find(vName) != runtimeVarLookup.top().end())
    {
        return runtimeVarLookup.top()[vName];
    }
    else
    {
        return runtimeGlobLookup[vName];
    }
}
void PcodeInterpreter::check_rtstack_size(const int n)
{
    if ((int)runtimeStack.size() < n)
    {
        logger.error("[%d] Runtime error: invalid stack depth = %d, wants %d", eip.top(), runtimeStack.size(), n);
        exit(-5);
    }
}
bool PcodeInterpreter::var_exists_new(const string varName){
    string vName;
    if (varName.find('[') == varName.npos)
    { //是变量
        vName = varName;
    }
    else
    { //是数组
        runtimeStack.push(0);
        check_rtstack_size(1);
        int _index = runtimeStack.top();
        runtimeStack.pop();
        vName = varName.substr(0, varName.find_first_of('[') + 1) + to_string(_index) + varName.substr(varName.find_first_of(']'));
    }
    if(eip.size()==1){
        if(runtimeVarLookup.top().find(vName) == runtimeVarLookup.top().end() && runtimeGlobLookup.find(vName) == runtimeGlobLookup.end()){
            return false;
        }
        return true;
    } else {
        if (runtimeVarLookup.top().find(vName) == runtimeVarLookup.top().end())
            {
                return false;
            }
        }
    return true;

}
bool PcodeInterpreter::var_exists(const string varName)
{
    string vName;
    if (varName.find('[') == varName.npos)
    { //是变量
        vName = varName;
    }
    else
    { //是数组
        runtimeStack.push(0);
        check_rtstack_size(1);
        int _index = runtimeStack.top();
        runtimeStack.pop();
        vName = varName.substr(0, varName.find_first_of('[') + 1) + to_string(_index) + varName.substr(varName.find_first_of(']'));
    }
    if(runtimeVarLookup.top().find(vName) == runtimeVarLookup.top().end() && runtimeGlobLookup.find(vName) == runtimeGlobLookup.end()){
            return false;
        }
    return true;
}
int PcodeInterpreter::do_arg(const string cmd)
{
    unordered_map<string, int> m;
    runtimeVarLookup.push(m);
    if (cmd == "")
    {
        return 0;
    }
    logger.info("cmd size: %d | %s", cmd.length(), cmd.c_str());
    vector<string> args;
    SplitString(cmd, args, ",");
    check_rtstack_size(args.size());
    reverse(args.begin(), args.end());
    for (auto a : args)
    {
        vector<string> attr;
        SplitString(a, attr, ":");
        string dtype = attr.size() < 2 ? "int" : attr[1];
        runtimeVarLookup.top()[attr[0]] = runtimeVar.size();
        runtimeVar.push_back({dtype, attr[0], runtimeStack.top()}); //type,name,val
        logger.info("arg:%s=%d", a.c_str(), runtimeStack.top());
        runtimeStack.pop();
    }
    return 0;
}
int PcodeInterpreter::do_ret(const string cmd)
{
    logger.info("ret command position 2: %s", cmd.c_str());
    if (cmd == "~")
    {
        int t = runtimeStack.top();
        runtimeStack.pop();
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for (int i = 0; i < (n - o - 1); i++)
        {
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        runtimeStack.push(t);
        //需要pop和push里记录用了几次，最后在这里运行时栈
        eip.pop();
        return 0;
    }
    else if (cmd == "")
    {
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for (int i = 0; i < (n - o - 1); i++)
        {
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        eip.pop();
        return 0;
    }
    else if (var_exists(cmd))
    {
        //for(int i=)
        int t = runtimeVar[runtimeVarLookup.top()[cmd]].val;
        int n = old_sp.top();
        old_sp.pop();
        int o = old_sp.top();
        for (int i = n; i > o; i--)
        {
            runtimeVar.pop_back();
        }
        runtimeVarLookup.pop();
        runtimeStack.push(t);
        eip.pop();
        return 0;
    }
    return 0;
}
int PcodeInterpreter::do_jmp(const string cmd)
{
    if (labelMap.find(cmd) != labelMap.end())
    {
        int tar = labelMap[cmd];
        logger.info("jump target:%d", tar);
        eip.top() = tar - 1;
    }
    else
    {
        logger.error("[%d] Runtime error: %s not found in label map.", eip.top(), cmd.c_str());
        exit(-6);
    }
    return 0;
}
int PcodeInterpreter::do_jz(const string cmd)
{
    check_rtstack_size(1);
    if (labelMap.find(cmd) == labelMap.end())
    {
        logger.error("[%d] Runtime error: %s not found in label map.", eip.top(), cmd.c_str());
        exit(-6);
    }
    int z = runtimeStack.top();
    runtimeStack.pop();
    if (z == 0)
    {
        int tar = labelMap[cmd];
        eip.top() = tar - 1;
    }
    else
    {
        return 0;
    }
    return 0;
}
int PcodeInterpreter::do_jnz(const string cmd)
{
    check_rtstack_size(1);
    if (labelMap.find(cmd) == labelMap.end())
    {
        logger.error("[%d] Runtime error: %s not found in label map.", eip.top(), cmd.c_str());
        exit(-6);
    }
    int z = runtimeStack.top();
    runtimeStack.pop();
    if (z != 0)
    {
        int tar = labelMap[cmd];
        eip.top() = tar - 1;
    }
    else
    {
        return 0;
    }
    return 0;
}
int PcodeInterpreter::do_print(const string cmd)
{
    vector<string> info;
    string output;
    stringstream sstream;
    SplitString(cmd, info, "`");
    if (cmd == "a")
    {
    }
    for (auto i : info)
    {
        if (i.front() == '\"' && i.back() == '\"')
        {
            sstream << i.substr(1, i.length() - 2);
        }
        else if (i.find('~') != i.npos)
        { //输出栈顶元素并pop
            check_rtstack_size(1);
            sstream << runtimeStack.top();
            runtimeStack.pop();
            //这里后续判断类型
        }
        else
        {
            sstream << runtimeVar[get_var(i)].val;
        }
    }
    sstream << endl;
    // if(this->toFile){
    //     fout << sstream.rdbuf();
    //     cout << "used print"<<endl;
    // } else {
    //     cout << sstream.rdbuf();
    // }
    *(this->out) << sstream.rdbuf();
    return 0;
}
int PcodeInterpreter::do_exit(const string cmd)
{
    logger.info("---bye---");
    if(this->out!=&cout){
        delete this->out;
        this->out = &cout;
    }
    exit(0);
}
int PcodeInterpreter::do_input(const string cmd)
{
    //<r_stmt> ::= scanf'('<ident>{,<ident>}')'
    vector<string> vars;
    SplitString(cmd, vars, ",");
    for (auto var : vars)
    {
        *(this->in) >> runtimeVar[get_var(var)].val;
        logger.debug("setting %s to %s", runtimeVar[get_var(var)].name.c_str(), to_string(runtimeVar[get_var(var)].val).c_str());
    }
    return 0;
}
int PcodeInterpreter::do_var(const string cmd)
{
    vector<string> vars;
    SplitString(cmd, vars, ",");
    for (auto var : vars)
    {
        vector<string> attr;
        SplitString(var, attr, ":");
        if (var_exists_new(attr[0]))
        {
            logger.error("[%d] Runtime error: duplicate definition of variable %s", eip.top(), var.c_str());
            exit(-1);
        }
        if (attr[0].find('[') != attr[0].npos)
        { //array mode
            int len = stoi(attr[0].substr(attr[0].find_first_of('[') + 1, attr[0].find_first_of(']') - 2));
            for (int i = 0; i < len; i++)
            {
                string vName = attr[0].substr(0, attr[0].find_first_of('[')) + "[" + to_string(i) + "]";
                if (eip.size() == 1)
                {
                    runtimeGlobLookup[vName] = runtimeVar.size();
                } else {
                    runtimeVarLookup.top()[vName] = runtimeVar.size();
                }
                runtimeVar.push_back({attr[1], attr[0], 0});
                old_sp.top()++;
            }
            logger.info("declared array variable %s (%d)", attr[0].substr(0, attr[0].find_first_of('[')).c_str(), len);
        }
        else
        {
            if (eip.size() == 1)
            {
                runtimeGlobLookup[attr[0]] = runtimeVar.size();
            } else {
                runtimeVarLookup.top()[attr[0]] = runtimeVar.size();
            }
            runtimeVar.push_back({attr[1], attr[0], 0});
            old_sp.top()++;
            logger.info("declared variable %s", attr[0].c_str());
        }
    }
    return 0;
}
int PcodeInterpreter::do_push(const string cmd)
{ //如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    int a;
    string msg;
    if (var_exists(cmd))
    {
        a = runtimeVar[get_var(cmd)].val;
        msg += "variable:";
        msg += cmd;
    }
    else
    {
        if (!isnum(cmd))
        {
            logger.error("[%d] Runtime error: invalid push argument %s", eip.top(), cmd.c_str());
            exit(-3);
        }
        stringstream ss;
        ss << cmd;
        ss >> a;
    }
    runtimeStack.push(a);
    logger.info("push: %d %s", a, msg.c_str());
    return 0;
}
int PcodeInterpreter::do_pop(const string cmd)
{ //如果发现是数组操作，去栈拿【i】：栈顶的是下标，运算数在第二个
    if (cmd == "")
    { //直接pop，不管了
        runtimeStack.pop();
        logger.info("direct pop");
        return 0;
    }
    int v = runtimeStack.top();
    runtimeStack.pop();
    runtimeVar[get_var(cmd)].val = v;

    logger.info("pop into %s", cmd.c_str());
    return -1;
}
int PcodeInterpreter::func_call(const string funcName)
{
    string fname = funcName.substr(1, funcName.length() - 1);
    if (funcMap.find(fname) != funcMap.end())
    {
        logger.info("eip pushed %d", funcMap[fname]);
        eip.push(funcMap[fname]);
        old_sp.push(old_sp.top() + 1);
    }
    else
    {
        logger.error("%s is not a valid funcname", fname.c_str());
        exit(-8);
    }
    return 0;
}
string PcodeInterpreter::interpret(const std::string &in_file_name, const std::string &out_file_name)
{
    ifstream myfile(in_file_name);
    string temp;
    if (!myfile.is_open())
    {
        logger.fatal("failed to open file %s",in_file_name.c_str());
        return "";
    }
    this->in = new ifstream("stdin.txt");
    if(out_file_name!=""){//使用cout
        //this->toFile = false;
        this->out = new ofstream(out_file_name);
    } else {
        // this->toFile = true;
        // this->fout.open(out_file_name);
        // if(fout.fail()){
        //     logger.fatal("failed to open output file %s",out_file_name.c_str());
        //     return "";
        // }
        // this->fout << "helo";
    }
    while (getline(myfile, temp))
    {
        temp = trim(temp);

        if (temp[0] == ';')
            continue;
        if (temp.empty())
            continue;
        vector<string> v;
        SplitString(temp, v, " ");
        this->code.push_back(v);
        if (v.size() > 0 && v[0].back() == ':')
        {
            string labelName = v[0].substr(0, v[0].size() - 1);
            if (labelMap.find(labelName) != labelMap.end())
            {
                logger.error("[%d] Runtime error: duplicate labels error.", eip.top());
                exit(-7);
            }
            this->labelMap.insert(make_pair(labelName, this->code.size()));
        }
        else if (v.size() == 2 && v[0] == "FUNC" && v[1].back() == ':')
        {
            string funcName = v[1].substr(1, v[1].size() - 2);
            if (funcMap.find(funcName) != funcMap.end())
            {
                logger.error("[%d] Runtime error: duplicate funcname error.", eip.top());
                exit(-7);
            }
            this->funcMap.insert(make_pair(funcName, this->code.size() - 1)); //回退到函数入口
        }
    }
    myfile.close();
    int i = 0;
    while (true)
    {
        vector<string> cmd = code[eip.top()];
        logger.info("cmd:---->%s", cmd[0].c_str());
        if (cmd.size() > 0)
        {
            if (cmd[0].back() == ':')
            {
                i++;
                eip.top()++;
                logger.info("is label,will continue");
                continue;
            }
            else if (cmd[0].front() == '$')
            {
                func_call(cmd[0]);
                i++;
                eip.top()++;
                continue;
            }
        }
        if (cmd.size() == 2)
        {
            (this->*cmdHandler[cmd[0]])(cmd[1]);
        }
        else
        {
            logger.info("[!]second command argument is empty!");
            (this->*cmdHandler[cmd[0]])("");
        }
        i++;
        eip.top()++;
    }
    return "";
}
#endif
