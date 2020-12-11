#ifndef TRANSLATOR_CPP
#define TRANSLATOR_CPP

#include "translator.h"
#include <string>
#include <fstream>
#include <iostream>

int GrammarTranslator::add_op() { return 0; }
int GrammarTranslator::mult_op() { return 0; }
int GrammarTranslator::rel_op() { return 0; }
int GrammarTranslator::letter() { return 0; }

int GrammarTranslator::udigit() { return 0; }
int GrammarTranslator::digit() { return 0; }
int GrammarTranslator::ch() { return 0; }
int GrammarTranslator::str() { return 0; }

int GrammarTranslator::type() { return 0; }
int GrammarTranslator::ident() { return 0; }

int GrammarTranslator::prog()
{
    if (word.first == "CONSTTK")
    {
        declare_const();
    }
    if (word.first == "INTTK" || word.first == "CHARTK")
    {
        get_word();
        if (word.first == "IDENFR")
        {
            get_word();
            if (word.first != "LPARENT")
            {
                roll_back(2);
                declare_var();
            }
            else
            {
                roll_back(2);
            }
        }
        else
        {
            roll_back(1);
        }
    }
    while (true)
    {
        if (word.first == "VOIDTK")
        {
            get_word();
            if (word.first == "MAINTK")
            {
                roll_back(1);
                main_f();
                break;
            }
            else
            {
                roll_back(1);
                f_void();
            }
        }
        else if (word.first == "INTTK" || word.first == "CHARTK")
        {
            f_ret();
        }
        else
        {
            logger.error("unknow \"%s\"", word.second.c_str());
            return -1;
        }
    }

    if (translate_type == "grammar")
    {
        fout << "<程序>" << std::endl;
    }
    return 0;
}

int GrammarTranslator::declare_const() { return 0; }
int GrammarTranslator::def_const() { return 0; }
int GrammarTranslator::declare_var() { return 0; }
int GrammarTranslator::def_var() { return 0; }

int GrammarTranslator::uinteger() { return 0; }
int GrammarTranslator::integer() { return 0; }

int GrammarTranslator::declare_h() { return 0; }
int GrammarTranslator::f_ret() { return 0; }
int GrammarTranslator::f_void() { return 0; }
int GrammarTranslator::param_table() { return 0; }

int GrammarTranslator::main_f() { return 0; }

int GrammarTranslator::comp_stmt() { return 0; }
int GrammarTranslator::stmt_list() { return 0; }
int GrammarTranslator::stmt() { return 0; }
int GrammarTranslator::eval() { return 0; }
int GrammarTranslator::cond_stmt() { return 0; }
int GrammarTranslator::cond() { return 0; }
int GrammarTranslator::loop_stmt() { return 0; }
int GrammarTranslator::step() { return 0; }

int GrammarTranslator::exp() { return 0; }
int GrammarTranslator::term() { return 0; }
int GrammarTranslator::factor() { return 0; }

int GrammarTranslator::f_ret_call() { return 0; }
int GrammarTranslator::f_void_call() { return 0; }
int GrammarTranslator::arg_list() { return 0; }

int GrammarTranslator::r_stmt() { return 0; }
int GrammarTranslator::w_stmt() { return 0; }
int GrammarTranslator::ret_stmt() { return 0; }

int GrammarTranslator::e_word()
{
    logger.error("e_word");
    return 0;
}
int GrammarTranslator::e_redifine_identifier()
{
    logger.error("e_redifine_identifier");
    return 0;
}
int GrammarTranslator::e_undifine_identifier()
{
    logger.error("e_undifine_identifier");
    return 0;
}
int GrammarTranslator::e_func_param_type()
{
    logger.error("e_func_param_type");
    return 0;
}
int GrammarTranslator::e_func_param_n()
{
    logger.error("e_func_param_n");
    return 0;
}
int GrammarTranslator::e_condition_type()
{
    logger.error("e_condition_type");
    return 0;
}
int GrammarTranslator::e_return_void()
{
    logger.error("e_return_void");
    return 0;
}
int GrammarTranslator::e_return_val()
{
    logger.error("e_return_val");
    return 0;
}
int GrammarTranslator::e_array_index()
{
    logger.error("e_array_index");
    return 0;
}
int GrammarTranslator::e_change_const_value()
{
    logger.error("e_change_const_value");
    return 0;
}
int GrammarTranslator::e_semicolon()
{
    logger.error("e_semicolon");
    return 0;
}
int GrammarTranslator::e_right_parenthesis()
{
    logger.error("e_right_parenthesis");
    return 0;
}
int GrammarTranslator::e_right_bracket()
{
    logger.error("e_right_bracket");
    return 0;
}
int GrammarTranslator::e_do_while()
{
    logger.error("e_do_while");
    return 0;
}
int GrammarTranslator::e_const_define_type()
{
    logger.error("e_const_define_type");
    return 0;
}

int GrammarTranslator::get_word()
{
    if (now_word_id == top_word_id)
    {
        if (bottom_word_id <= top_word_id - WORD_BUFFER_SZ + 1)
            bottom_word_id++;
        top_word_id++;
        word_buffer[top_word_id % WORD_BUFFER_SZ] = words.get_word();
    }
    now_word_id++;
    word = word_buffer[now_word_id % WORD_BUFFER_SZ];
    return 0;
}
int GrammarTranslator::roll_back(int step)
{
    if (step < 0)
    {
        logger.error("wrong roll back step %d", step);
        return -1;
    }
    if (now_word_id - step <= bottom_word_id)
    {
        logger.error("roll back too much");
        return -1;
    }
    now_word_id -= step;
    word = word_buffer[now_word_id];
    return 0;
}

GrammarTranslator::GrammarTranslator()
{
    loaded_lex = false;
    bottom_word_id = 0;
    top_word_id = -1;
    now_word_id = -1;
}
int GrammarTranslator::load_lexical(std::istream &file)
{
    logger.debug("loading keyword and type");
    std::string type, keyword;
    while (!loaded_lex && file >> type >> keyword)
    {
        logger.debug("get %s %s", keyword.c_str(), type.c_str());
        if (type == "define" && keyword == "words")
        {
            while (file >> type >> keyword)
            {
                logger.debug("get %s %s", keyword.c_str(), type.c_str());
                if (type == "define")
                {
                    loaded_lex = true;
                    break;
                }
                words.append_keyword(keyword, type);
            }
        }
    }
    logger.debug("load lexical done");
    return 0;
}
int GrammarTranslator::load_lexical(const std::string &file_name)
{
    logger.debug("load lexical file name");
    if (loaded_lex)
    {
        logger.error("loaded lexical before");
        return -1;
    }
    std::ifstream file;
    file.open(file_name);

    load_lexical(file);

    file.close();

    return 0;
}
int GrammarTranslator::translate_lexical(const std::string &in_file_name, const std::string &out_file_name)
{
    words.open(in_file_name);
    fout.open(out_file_name);
    line_number = 0;

    get_word();
    while (word.first != "")
    {
        fout << word.first << " " << word.second << std::endl;
        get_word();
    }
    words.close();
    fout.close();
    return 0;
}
int GrammarTranslator::translate(const std::string &in_file_name,
                                 const std::string &out_file_name, const std::string &type)
{
    words.open(in_file_name);
    fout.open(out_file_name);
    translate_type = type;
    line_number = 0;
    int e;

    get_word();
    e = prog();
    if (e == -1)
    {
        logger.error("unknow \"%s\"", word.second.c_str());
        return -1;
    }
    words.close();
    fout.close();
    return 0;
}

#endif