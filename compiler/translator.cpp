#ifndef TRANSLATOR_CPP
#define TRANSLATOR_CPP

#include "translator.h"
#include <string>
#include <fstream>
#include <iostream>

/**********************************
 * 
 * grammartical analysis
 * 
 **********************************/

int GrammarTranslator::prog()
{
    if (word.first == "CONSTTK")
    {
        declare_const();
    }
    if (!(detect(3, "INTTK", "IDENFR", "LPARENT") || detect(3, "CHARTK", "IDENFR", "LPARENT")))
    {
        declare_var();
    }
    while (true)
    {
        if (detect(2, "VOIDTK", "MAINTK"))
        {
            main_f();
            break;
        }
        else if (word.first == "VOIDTK")
        {
            f_void();
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

    print_grammar("<程序>");
    return 0;
}

int GrammarTranslator::declare_const()
{
    while (word.first == "CONSTTK")
    {
        get_word();
        def_const();
        if (word.first == "SEMICN")
        {
            get_word();
        }
        else
        {
            e_semicolon();
        }
    }

    print_grammar("<常量说明>");
    return 0;
}
int GrammarTranslator::def_const()
{
    Word type;
    Word idenfr;
    if (word.first == "INTTK" || word.first == "CHARTK")
    {
        type = word;
    }
    else
    {
        logger.error("wrong type %s", word.second.c_str());
        return -1;
    }
    get_word();

    while (true)
    {
        if (word.first == "IDENFR")
        {
            idenfr = word;
        }
        else
        {
            logger.error("missing identifier after %s", type.second.c_str());
            return -1;
        }
        get_word();

        if (word.first != "ASSIGN")
        {
            logger.error("missing '=' after %s", idenfr.second.c_str());
            return -1;
        }
        get_word();

        if (word.first == "INTCON" || word.first == "CHARCON")
        {
            //gmc type idenfr=word
        }
        else
        {
            e_const_define_type();
        }
        get_word();

        if (word.first == "COMMA")
        {
            get_word();
            continue;
        }
        else
        {
            break;
        }
    }

    print_grammar("<常量定义>");
    return 0;
}
int GrammarTranslator::declare_var()
{
    print_grammar("<变量说明>");
    return 0;
}
int GrammarTranslator::def_var()
{
    print_grammar("<变量定义>");
    return 0;
}

int GrammarTranslator::uinteger()
{
    print_grammar("<无符号整数>");
    return 0;
}
int GrammarTranslator::integer()
{
    print_grammar("<整数>");
    return 0;
}

int GrammarTranslator::declare_h()
{
    print_grammar("<声明头部>");
    return 0;
}
int GrammarTranslator::f_ret()
{
    print_grammar("<有返回值函数定义>");
    return 0;
}
int GrammarTranslator::f_void()
{
    print_grammar("<无返回值函数定义>");
    return 0;
}
int GrammarTranslator::param_table()
{
    print_grammar("<参数表>");
    return 0;
}

int GrammarTranslator::main_f()
{
    print_grammar("<主函数>");
    return 0;
}

int GrammarTranslator::comp_stmt()
{
    print_grammar("<复合语句>");
    return 0;
}
int GrammarTranslator::stmt_list()
{
    print_grammar("<语句列>");
    return 0;
}
int GrammarTranslator::stmt()
{
    print_grammar("<语句>");
    return 0;
}
int GrammarTranslator::eval()
{
    print_grammar("<赋值语句>");
    return 0;
}
int GrammarTranslator::cond_stmt()
{
    print_grammar("<条件语句>");
    return 0;
}
int GrammarTranslator::cond()
{
    print_grammar("<条件>");
    return 0;
}
int GrammarTranslator::loop_stmt()
{
    print_grammar("<循环语句>");
    return 0;
}
int GrammarTranslator::step()
{
    print_grammar("<步长>");
    return 0;
}

int GrammarTranslator::exp()
{
    print_grammar("<表达式>");
    return 0;
}
int GrammarTranslator::term()
{
    print_grammar("<项>");
    return 0;
}
int GrammarTranslator::factor()
{
    print_grammar("<因子>");
    return 0;
}

int GrammarTranslator::f_ret_call()
{
    print_grammar("<有返回值函数调用语句>");
    return 0;
}
int GrammarTranslator::f_void_call()
{
    print_grammar("<无返回值函数调用语句>");
    return 0;
}
int GrammarTranslator::arg_list()
{
    print_grammar("<值参数表>");
    return 0;
}

int GrammarTranslator::r_stmt()
{
    print_grammar("<读语句>");
    return 0;
}
int GrammarTranslator::w_stmt()
{
    print_grammar("<写语句>");
    return 0;
}
int GrammarTranslator::ret_stmt()
{
    print_grammar("<返回语句>");
    return 0;
}

/*******************************
 * 
 * error handling
 * 
 *******************************/

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

int GrammarTranslator::print_grammar(const std::string &info)
{
    if (translate_type == "grammar")
    {
        fout << info << std::endl;
    }
    return 0;
}
int GrammarTranslator::print_error(const int &error_line_number, const std::string &error_type)
{
    if (translate_type == "error")
    {
        fout << error_line_number << ' ' << error_type << std::endl;
    }
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
bool GrammarTranslator::detect(int step, ...)
{
    char *rule;
    va_list args;
    va_start(args, step);

    rule = va_arg(args, char *);
    if (word.first != rule)
    {
        return false;
    }
    for (int i = 1; i < step; i++)
    {
        rule = va_arg(args, char *);
        get_word();
        if (word.first != rule)
        {
            roll_back(i);
            return false;
        }
    }

    va_end(args);
    roll_back(step - 1);
    return true;
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