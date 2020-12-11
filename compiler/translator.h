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

    int add_op();  // 加法运算符
    int mult_op(); // 乘法运算符
    int rel_op();  // 关系运算符
    int letter();  // 字母

    int udigit(); // 非零数字
    int digit();  // 数字
    int ch();     // 字符
    int str();    // 字符串

    int type();  // 类型标识符
    int ident(); // 标识符

    int prog(); // 程序

    int declare_const(); // 常量说明
    int def_const();     // 常量定义
    int declare_var();   // 变量说明
    int def_var();       // 变量定义

    int uinteger(); // 无符号整数
    int integer();  // 整数

    int declare_h();   // 声明头部
    int f_ret();       // 有返回值函数定义
    int f_void();      // 无返回值函数定义
    int param_table(); // 参数表

    int main_f(); // 主函数

    int comp_stmt(); // 复合语句
    int stmt_list(); // 语句列
    int stmt();      // 语句
    int eval();      // 赋值语句
    int cond_stmt(); // 条件语句
    int cond();      // 条件
    int loop_stmt(); // 循环语句
    int step();      // 步长

    int exp();    // 表达式
    int term();   // 项
    int factor(); // 因子

    int f_ret_call();  // 有返回值函数调用语句
    int f_void_call(); // 无返回值函数调用语句
    int arg_list();    // 值参数表

    int r_stmt();   // 读语句
    int w_stmt();   // 写语句
    int ret_stmt(); // 返回语句

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