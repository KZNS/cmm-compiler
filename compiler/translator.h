#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "automaton.h"
#include "cLogger/clogger.h"
#include <utility>

#define WORD_BUFFER_SZ 10

extern Logger logger;

class GrammarTranslator
{
private:
    LexicalAutomaton words;
    bool loaded_lex;
    std::ofstream fout;
    Word word;
    Word word_buffer[WORD_BUFFER_SZ];
    int bottom_word_id, now_word_id, top_word_id;
    int line_number;
    std::string translate_type;

    int add_op();
    int mult_op();
    int rel_op();
    int letter();

    int udigit();
    int digit();
    int ch();
    int str();

    int type();
    int ident();

    int prog();

    int declare_const();
    int def_const();
    int declare_var();
    int def_var();

    int uinteger();
    int integer();

    int declare_h();
    int f_ret();
    int f_void();
    int param_table();

    int main_f();

    int comp_stmt();
    int stmt_list();
    int stmt();
    int eval();
    int cond_stmt();
    int cond();
    int loop_stmt();
    int step();

    int exp();
    int term();
    int factor();

    int f_ret_call();
    int f_void_call();
    int arg_list();

    int r_stmt();
    int w_stmt();
    int ret_stmt();

    int e_word();                //非法符号或不符合词法
    int e_redifine_identifier(); //名字重定义
    int e_undifine_identifier(); //未定义的名字
    int e_func_param_type();     //函数参数个数不匹配
    int e_func_param_n();        //函数参数类型不匹配
    int e_condition_type();      //条件判断中出现不合法的类型
    int e_return_void();         //无返回值的函数存在不匹配的return语句
    int e_return_val();          //有返回值的函数缺少return语句或存在不匹配的return语句
    int e_array_index();         //数组元素的下标只能是整型表达式
    int e_change_const_value();  //不能改变常量的值
    int e_semicolon();           //应为分号
    int e_right_parenthesis();   //应为右小括号’)’
    int e_right_bracket();       //应为右中括号’]’
    int e_do_while();            //do-while应为语句中缺少while
    int e_const_define_type();   //常量定义中=后面只能是整型或字符型常量

    int get_word();
    int roll_back(int step);

public:
    GrammarTranslator();
    int load_lexical(std::istream &file);
    int load_lexical(const std::string &file_name);
    int translate_lexical(const std::string &in_file_name, const std::string &out_file_name);
    int translate(const std::string &in_file_name, const std::string &out_file_name,
                  const std::string &type = "PCODE");
};

#include "translator.cpp"

#endif