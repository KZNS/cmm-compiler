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
    std::stack<std::unordered_map<std::string,int>> runtimeVarLookup;
    std::unordered_map<std::string,Variable> globalVar;//全局变量表
    std::stack<int> runtimeStack;//stack运算栈：stack,int
    std::unordered_map<std::string,int> labelMap;//label表：label名对应跳到第几行代码
    std::unordered_map<std::string,int> funcMap;//函数表：？指示函数ret到哪行 注意函数有返回值则会往栈里放东西
    std::vector<std::vector<std::string>> code;//code代码区：暂定pair
    std::unordered_map<std::string,int(PcodeInterpreter::*)(const std::string)> cmdHandler;
    //int eip;//eip指针：int
    std::stack<int> eip;
    std::stack<int> old_sp;
    int do_var(const std::string);
    int do_push(const std::string);
    int do_pop(const std::string);
    int do_add(const std::string);
    int do_mul(const std::string);
    int do_print(const std::string);
    int do_input(const std::string);
    int do_jmp(const std::string);
    int do_jz(const std::string);
    int do_exit(const std::string);
    int do_ret(const std::string);
    int do_arg(const std::string);
    int func_call(const std::string);
    int get_var(const std::string);
    bool var_exists(const std::string);
    void check_rtstack_size(const int);
public:
    PcodeInterpreter();
    int interpret(const std::string &in_file_name);
};

#include "interpreter.cpp"

#endif
