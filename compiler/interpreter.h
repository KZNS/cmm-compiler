#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "cLogger/clogger.h"
#include <cstdarg>
#include <utility>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
extern Logger logger;

struct Variable
{
    std::string type;
    std::string name;
    int val;
};


class PcodeInterpreter
{
private:
    std::vector<Variable> runtimeVar;//变量表：？
    std::unordered_map<std::string,bool> runtimeVarLookup;
    std::unordered_map<std::string,Variable> globalVar;//全局变量表
    std::stack<int> runtimeStack;//stack运算栈：stack,int
    std::unordered_map<std::string,int> labelMap;//label表：label名对应跳到第几行代码
    std::unordered_map<std::string,int> funcMap;//函数表：？指示函数ret到哪行 注意函数有返回值则会往栈里放东西
    std::vector<std::vector<std::string>> code;//code代码区：暂定pair
    std::unordered_map<std::string,int(PcodeInterpreter::*)(const std::string)> cmdHandler;
    int eip;//eip指针：int
    int old_sp;//old_Stack_pointer
    int do_var(const std::string);
    int do_push(const std::string);
    int do_pop(const std::string);
    int do_add(const std::string);
    int do_mul(const std::string);
    int do_print(const std::string);
    int do_input(const std::string);
    
    
public:
    PcodeInterpreter();
    int interpret(const std::string &in_file_name);
};

#include "interpreter.cpp"

#endif
