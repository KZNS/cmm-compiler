#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "automaton.h"
#include "symbol_table.h"
#include "cLogger/clogger.h"
#include <utility>
#include <cstdarg>

#define WORD_BUFFER_SZ 10

extern Logger logger;

class GrammarTranslator
{
private:
    LexicalAutomaton words;
    SymbolTable table;
    bool loaded_lex;
    std::ostream *out;
    Word word;
    Word word_buffer[WORD_BUFFER_SZ];
    int line_number_buffer[WORD_BUFFER_SZ];
    int bottom_word_id, now_word_id, top_word_id;
    int last_line_number;
    int line_number;
    std::string translate_type;

    int pcode_indent_deep;

    int prog(); // 程序

    int declare_const(); // 常量说明
    int def_const();     // 常量定义
    int declare_var();   // 变量说明
    int def_var();       // 变量定义

    int uinteger(unsigned int &x); // 无符号整数
    int integer(int &x);           // 整数

    int declare_h(std::string &type, std::string &name); // 声明头部
    int f_ret();                                         // 有返回值函数定义
    int f_void();                                        // 无返回值函数定义
    int param_table(std::vector<VarProperty> &arg_list); // 参数表

    int main_f(); // 主函数

    int comp_stmt(std::string ret_type, bool &returned); // 复合语句
    int stmt_list(std::string ret_type, bool &returned); // 语句列
    int stmt(std::string ret_type, bool &returned);      // 语句
    int eval();                                          // 赋值语句
    int cond_stmt(std::string ret_type, bool &returned); // 条件语句
    int cond();                                          // 条件
    int loop_stmt(std::string ret_type, bool &returned); // 循环语句
    int step(unsigned int &x);                           // 步长

    int exp(std::string &type);    // 表达式
    int term(std::string &type);   // 项
    int factor(std::string &type); // 因子

    int f_ret_call(std::string &ret_type);            // 有返回值函数调用语句
    int f_void_call();                                // 无返回值函数调用语句
    int arg_list(std::vector<VarProperty> &arg_list); // 值参数表

    int r_stmt();                       // 读语句
    int w_stmt();                       // 写语句
    int ret_stmt(std::string ret_type); // 返回语句

    int str_const(std::string &str); // 字符串

    int e_word();                // a 非法符号或不符合词法
    int e_redifine_identifier(); // b 名字重定义
    int e_undifine_identifier(); // c 未定义的名字
    int e_func_param_n();        // d 函数参数个数不匹配
    int e_func_param_type();     // e 函数参数类型不匹配
    int e_condition_type();      // f 条件判断中出现不合法的类型
    int e_return_void();         // g 无返回值的函数存在不匹配的return语句
    int e_return_val();          // h 有返回值的函数缺少return语句或存在不匹配的return语句
    int e_array_index();         // i 数组元素的下标只能是整型表达式
    int e_change_const_value();  // j 不能改变常量的值
    int e_semicolon();           // k 应为分号
    int e_right_parenthesis();   // l 应为右小括号’)’
    int e_right_bracket();       // m 应为右中括号’]’
    int e_do_while();            // n do-while应为语句中缺少while
    int e_const_define_type();   // o 常量定义中=后面只能是整型或字符型常量

    int print_lexical(const Word &p_word);                                        //输出词法信息
    int print_grammar(const std::string &info);                                   //输出语法信息
    int print_error(const int &error_line_number, const std::string &error_type); //输出错误信息
    int print_pcode(std::string format, ...);                                     //输出pcode

    int change_pcode_indent_deep(int x);
    std::string get_unique_label();

    int get_word();
    int get_new_word();
    int roll_back(int step);
    bool detect(int step, ...);

    int translate(const std::string &type);

public:
    GrammarTranslator();
    int load_lexical(std::istream &file);
    int load_lexical(const std::string &file_name);
    int translate(std::istream &in_stream, std::ostream &out_stream,
                  const std::string &type = "PCODE");
    int translate(const std::string &in_file_name, const std::string &out_file_name,
                  const std::string &type = "PCODE");
};

#include "translator.cpp"

#endif