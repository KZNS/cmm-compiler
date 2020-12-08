#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "automaton.h"
#include "cLogger/clogger.h"
#include <utility>

extern Logger logger;

class GrammarTranslator
{
private:
    LexicalAutomaton words;
    bool loaded_lex;
    std::ofstream fout;
    Word word, next_word;
    int line_number;

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

    int get_word();

public:
    GrammarTranslator();
    int load_lexical(std::istream &file);
    int load_lexical(const std::string &file_name);
    int translate_lexical(const std::string &in_file_name, const std::string &out_file_name);
    int translate(const std::string &in_file_name, const std::string &out_file_name);
};

#include "translator.cpp"

#endif