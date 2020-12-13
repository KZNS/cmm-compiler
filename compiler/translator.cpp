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

/**
 * 程序
 * <prog> ::= [<declare_const>][<declare_var>]{<f_ret>|<f_void>}<main_f>
 */
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
            logger.error("unknown \"%s\"", word.second.c_str());
            return -1;
        }
    }

    print_grammar("<程序>");
    return 0;
}

/**
 * 常量说明
 * <declare_const> ::= const<def_const>;{const<def_const>;}
 */
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
/**
 * 常量定义
 * <def_const> ::= int<ident>=<integer>{,<ident>=<integer>}
 *                 char<ident>=<ch>{,<ident>=<ch>}
 */
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
/**
 * 变量说明
 * <declare_var> ::= <def_var>;{<def_var>;}
 */
int GrammarTranslator::declare_var()
{
    while (!(detect(3, "INTTK", "IDENFR", "LPARENT") || detect(3, "CHARTK", "IDENFR", "LPARENT")))
    {
        def_var();
        if (word.first == "SEMICN")
        {
            get_word();
        }
        else
        {
            e_semicolon();
        }
    }

    print_grammar("<变量说明>");
    return 0;
}
/**
 * 变量定义
 * <def_var> ::= <type>(<ident>|<ident>'['<uinteger>']'){,(<ident>|<ident>'['<uinteger>']')}
 */
int GrammarTranslator::def_var()
{
    print_grammar("<变量定义>");
    return 0;
}

/**
 * 无符号整数
 * <uinteger> ::= <udigit>{<digit>}
 *                0
 */
int GrammarTranslator::uinteger(unsigned int &x)
{
    if (word.first == "INTCON")
    {
        if (word.second.length() < 1 || (word.second.length() > 1 && word.second[0] == '0'))
        {
            e_word();
            return -1;
        }
        else
        {
            sscanf(word.second.c_str(), "%u", &x);
        }
    }
    else
    {
        logger.error("missing uinteger");
        return -1;
    }
    get_word();

    print_grammar("<无符号整数>");
    return 0;
}
/**
 * 整数
 * <integer> ::= [+|-]<uinteger>
 */
int GrammarTranslator::integer(int &x)
{
    if (word.first == "PLUS")
    {
        x = 1;
        get_word();
    }
    else if (word.first == "MINU")
    {
        x = -1;
        get_word();
    }

    unsigned int ux;
    int e;
    e = uinteger(ux);
    if (e)
    {
        return -1;
    }
    x *= ux;
    get_word();

    print_grammar("<整数>");
    return 0;
}

/**
 * 声明头部
 * <declare_h> ::= int<ident>
 *                 char<ident>
 */
int GrammarTranslator::declare_h()
{
    print_grammar("<声明头部>");
    return 0;
}
/**
 * 有返回值函数定义
 * <f_ret> ::= <declare_h>'('<param_table>')''{'<comp_stmt>'}'
 */
int GrammarTranslator::f_ret()
{
    print_grammar("<有返回值函数定义>");
    return 0;
}
/**
 * 无返回值函数定义
 * <f_void> ::= void<ident>'('<param_table>')''{'<comp_stmt>'}'
 */
int GrammarTranslator::f_void()
{
    print_grammar("<无返回值函数定义>");
    return 0;
}
/**
 * 参数表
 * <param_table> ::= <type><ident>{,<type><ident>}
 *                   <空>
 */
int GrammarTranslator::param_table()
{
    print_grammar("<参数表>");
    return 0;
}

/**
 * 主函数
 * <main_f> ::= voidmain‘(’‘)’‘{’<comp_stmt>‘}’
 */
int GrammarTranslator::main_f()
{
    if (word.first == "VOIDTK")
    {
        get_word();
    }
    else
    {
        logger.error("VOIDTK missing in main_f");
        return -1;
    }
    if (word.first == "MAINTK")
    {
        get_word();
    }
    else
    {
        logger.error("MAINTK missing in main_f");
        return -1;
    }
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("LPARENT missing in main_f");
        return -1;
    }
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("RPARENT missing in main_f");
        return -1;
    }
    if (word.first == "LBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("LBRACE missing in main_f");
        return -1;
    }
    comp_stmt();
    if (word.first == "RBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("RBRACE missing in main_f");
        return -1;
    }
    print_grammar("<主函数>");
    return 0;
}

/**
 * 复合语句
 * <comp_stmt> ::= [<declare_const>][<declare_var>]<stmt_list>
 */
int GrammarTranslator::comp_stmt()
{
    if (word.first == "CONSTTK")
    {
        declare_const();
    }
    if (!(detect(3, "INTTK", "IDENFR", "LPARENT") || detect(3, "CHARTK", "IDENFR", "LPARENT")))
    {
        declare_var();
    }
    stmt_list();
    print_grammar("<复合语句>");
    return 0;
}
/**
 * 语句列
 * <stmt_list> ::= {<stmt>}
 */
int GrammarTranslator::stmt_list()
{
    while (true)
    { //??????
        if (word.first == "IFTK" || word.first == "DOTK" || word.first == "WHILETK" ||
            word.first == "FORTK" || word.first == "LBRACE" || word.first == "IDENFR" ||
            word.first == "SCANFTK" || word.first == "PRINTFTK" || word.first == "RETURNTK" ||
            word.first == "SEMICN")
        {
            stmt();
        }
        else
        {
            break;
        }
    }
    print_grammar("<语句列>");
    return 0;
}
/**
 * 语句
 * <stmt> ::= <cond_stmt>
 *            <loop_stmt>
 *            '{'<stmt_list>'}'
 *            <f_ret_call>;
 *            <f_void_call>;
 *            <eval>;
 *            <r_stmt>;
 *            <w_stmt>;
 *            <空>;
 *            <ret_stmt>;
 */
int GrammarTranslator::stmt()
{
    if (word.first == "IFTK")
    {
        cond_stmt();
    }
    else if (word.first == "DOTK" || word.first == "WHILETK" || word.first == "FORTK")
    {
        loop_stmt();
    }
    else if (word.first == "LBRACE")
    {
        get_word();
        stmt_list();
        if (word.first == "RBRACE")
        {
            get_word();
        }
        else
        {
            logger.error("rbrace missing in stmt() {stmt_list}");
            return -1;
        }
    }
    else if (detect(2, "IDENFR", "LPARENT"))
    {
        //??如何区分有无返回值
    }
    else if (word.first == "IDENFR")
    {
        eval();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in stmt eval");
            return -1;
        }
        get_word();
    }
    else if (word.first == "SCANFTK")
    {
        r_stmt();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in stmt scanftk");
            return -1;
        }
        get_word();
    }
    else if (word.first == "PRINTFTK")
    {
        w_stmt();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in stmt printftk");
            return -1;
        }
        get_word();
    }
    else if (word.first == "RETURNTK")
    {
        ret_stmt();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in stmt returntk");
            return -1;
        }
        get_word();
    }
    else if (word.first == "SEMICN")
    {
        get_word();
    }
    else
    {
        logger.error("invalid syntax in stmt");
        return -1;
    }
    print_grammar("<语句>");
    return 0;
}
/**
 * 赋值语句
 * <eval> ::= <ident>=<exp>
 *            <ident>'['<exp>']'=<exp>
 */
int GrammarTranslator::eval()
{
    if (word.first == "IDENFR")
    {
        get_word();
    }
    else
    {
        logger.error("missing idenfer in eval");
        return -1;
    }
    if (word.first == "LBRACK")
    {
        get_word();
        exp();
        if (word.first == "RBRACK")
        {
            get_word();
        }
        else
        {
            logger.error("missing right bracket in eval");
            return -1;
        }
    }
    if (word.first == "ASSIGN")
    {
        get_word();
        exp();
    }
    else
    {
        logger.error("missing assign in eval");
        return -1;
    }
    print_grammar("<赋值语句>");
    return 0;
}
/**
 * 条件语句
 * <cond_stmt> ::= if'('<cond>')'<stmt>[else<stmt>]
 */
int GrammarTranslator::cond_stmt()
{
    if (word.first == "IFTK")
    {
        get_word();
    }
    else
    {
        logger.error("`if` missing in cond_stmt");
        return -1;
    }
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("`lparent` missing in cond_stmt");
        return -1;
    }
    cond();
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("`rparent` missing in cond_stmt");
        return -1;
    }
    stmt();
    if (detect(1, "ELSETK"))
    {
        get_word();
        stmt();
    }
    print_grammar("<条件语句>");
    return 0;
}
/**
 * 条件
 * <cond> ::= <exp><rel_op><exp>
 *            <exp>
 */
int GrammarTranslator::cond()
{
    exp();
    if (word.first == "LSS" || word.first == "LEQ" || word.first == "GRE" || word.first == "GEQ" || word.first == "EQL" || word.first == "NEQ")
    {
        get_word(); //rel_op();
        exp();
    }
    print_grammar("<条件>");
    return 0;
}
/**
 * 循环语句
 * <loop_stmt> ::= while'('<cond>')'<stmt>
 *                 do<stmt>while'('<cond>')'
 *                 for'('<ident>=<exp>;<cond>;<ident>=<ident>(+|-)<step>')'<stmt>
 */
int GrammarTranslator::loop_stmt()
{
    if (word.first == "WHILETK")
    {
        get_word();
        if (word.first == "LPARENT")
        {
            get_word();
        }
        else
        {
            logger.error("lparent missing in while of loop_stmp");
            return -1;
        }
        cond();
        if (word.first == "RPARENT")
        {
            get_word();
        }
        else
        {
            logger.error("rparent missing in while of loop_stmp");
            return -1;
        }
        stmt();
    }
    else if (word.first == "DOTK")
    {
        stmt();
        if (word.first == "WHILETK")
        {
            get_word();
        }
        else
        {
            logger.error("whiletk missing in for of loop_stmp");
            return -1;
        }
        if (word.first == "LPARENT")
        {
            get_word();
        }
        else
        {
            logger.error("lparent missing in dowhile of loop_stmp");
            return -1;
        }
        cond();
        if (word.first == "RPARENT")
        {
            get_word();
        }
        else
        {
            logger.error("rparent missing in dowhile of loop_stmp");
            return -1;
        }
    }
    else if (word.first == "FORTK")
    {
        get_word();
        if (word.first != "LPARENT")
        {
            logger.error("lparent missing in while of loop_stmp");
            return -1;
        }
        get_word();
        if (word.first != "IDENFR")
        {
            logger.error("idenfer missing in part1 of for in loop_stmt");
            return -1;
        }
        get_word();
        if (word.first != "ASSIGN")
        {
            logger.error("assign missing in part1 of for in loop_stmt");
            return -1;
        }
        get_word();
        exp();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in part1 of for in loop_stmt");
        }
        get_word();
        cond();
        if (word.first != "SEMICN")
        {
            logger.error("semicn missing in part2 of for in loop_stmt");
        }
        get_word();
        if (word.first != "IDENFR")
        {
            logger.error("idenfer missing in part3 of for in loop_stmt");
            return -1;
        }
        get_word();
        if (word.first != "ASSIGN")
        {
            logger.error("assign missing in part3 of for in loop_stmt");
            return -1;
        }
        get_word();
        if (word.first != "IDENFR")
        {
            logger.error("idenfer missing in part3 of for in loop_stmt");
            return -1;
        }
        get_word();
        if (word.first != "PLUS" && word.first != "MINU")
        {
            logger.error("(+|-) missing in part3 of for in loop_stmt");
            return -1;
        }
        get_word();
        step();
        if (word.first != "RPARENT")
        {
            logger.error("rparent missing in while of loop_stmp");
            return -1;
        }
        get_word();
        stmt();
    }
    else
    {
        logger.error("whiletk or dotk or fortk missing in loop_stmp");
        return -1;
    }
    print_grammar("<循环语句>");
    return 0;
}
/**
 * 步长
 * <step> ::= <uinteger>
 */
int GrammarTranslator::step()
{
    unsigned int x;
    int e;
    e = uinteger(x);
    if (e)
    {
        return -1;
    }

    print_grammar("<步长>");
    return 0;
}

/**
 * 表达式
 * <exp> ::= [+|-]<term>{<add_op><term>}
 */
int GrammarTranslator::exp()
{
    if (word.first == "PLUS" || word.first == "MINU")
    {
        get_word();
    }
    term();
    print_grammar("<表达式>");
    return 0;
}
/**
 * 项
 * <term> ::= <factor>{<mult_op><factor>}
 */
int GrammarTranslator::term()
{
    while (true)
    {
        factor();
        if (word.first == "MULT" || word.first == "DIV")
        {
            get_word();
            continue;
        }
        else
        {
            break;
        }
    }
    print_grammar("<项>");
    return 0;
}
/**
 * 因子
 * <factor> ::= <ident>
 *              <ident>'['<exp>']'
 *              '('<exp>')'
 *              <integer>
 *              <ch>
 *              <f_ret_call>
 */
int GrammarTranslator::factor()
{
    if (word.first == "IDENFR")
    {
        get_word();
        if (word.first == "LBRACK")
        {
            get_word();
            exp();
            if (word.first == "RBRACK")
            {
                get_word();
            }
            else
            {
                logger.error("missing rbrack in factor <ident>");
                return -1;
            }
        }
    }
    else if (word.first == "LPARENT")
    {
        get_word();
        exp();
        if (word.first == "RPARENT")
        {
            get_word();
        }
        else
        {
            logger.error("missing rparent in factor <(exp)>");
            return -1;
        }
    }
    else if (word.first == "CHARCON")
    {
        get_word();
    }
    else if (word.first == "PLUS" || word.first == "MINU" || word.first == "INTCON")
    { //??integer is broken
        int x;
        integer(x);
    }
    else
    {
        f_ret_call();
    }
    print_grammar("<因子>");
    return 0;
}

/**
 * 有返回值函数调用语句
 * <f_ret_call> ::= <ident>'('<arg_list>')'
 */
int GrammarTranslator::f_ret_call() //??没地方接返回值?
{
    if (word.first != "IDENFR")
    {
        logger.error("ident missing in f_ret_call");
        return -1;
    }
    get_word();
    if (word.first != "LPARENT")
    {
        logger.error("lparent missing in f_ret_call");
        return -1;
    }
    get_word();
    arg_list();
    if (word.first != "RPARENT")
    {
        logger.error("rparent missing in f_ret_call");
        return -1;
    }
    get_word();
    print_grammar("<有返回值函数调用语句>");
    return 0;
}
/**
 * 无返回值函数调用语句
 * <f_void_call> ::= <ident>'('<arg_list>')'
 */
int GrammarTranslator::f_void_call()
{
    if (word.first != "IDENFR")
    {
        logger.error("ident missing in f_void_call");
        return -1;
    }
    get_word();
    if (word.first != "LPARENT")
    {
        logger.error("lparent missing in f_void_call");
        return -1;
    }
    get_word();
    arg_list();
    if (word.first != "RPARENT")
    {
        logger.error("rparent missing in f_void_call");
        return -1;
    }
    get_word();
    print_grammar("<无返回值函数调用语句>");
    return 0;
}
/**
 * 值参数表
 * <arg_list> ::= <exp>{,<exp>}
 *                <空>
 */
int GrammarTranslator::arg_list()
{
    while (true)
    {
        exp();
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
    print_grammar("<值参数表>");
    return 0;
}

/**
 * 读语句
 * <r_stmt> ::= scanf'('<ident>{,<ident>}')'
 */
int GrammarTranslator::r_stmt()
{
    if (word.first != "SCANFTK")
    {
        logger.error("scanftk missing in r_stmt");
        return -1;
    }
    get_word();
    if (word.first != "LPARENT")
    {
        logger.error("lparent missing in r_stmt");
        return -1;
    }
    get_word();
    while (true)
    {
        if (word.first != "IDENFR")
        {
            logger.error("idenfr missing in r_stmt");
            return -1;
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
    if (word.first != "RPARENT")
    {
        logger.error("rparent missing in r_stmt");
        return -1;
    }
    get_word();
    print_grammar("<读语句>");
    return 0;
}
/**
 * 写语句
 * <w_stmt> ::= printf'('<str>,<exp>')'
 *              printf'('<str>')'
 *              printf'('<exp>')'
 */
int GrammarTranslator::w_stmt()
{
    if (word.first != "PRINTFTK")
    {
        logger.error("printtk missing in w_stmt");
        return -1;
    }
    get_word();
    if (word.first != "LPARENT")
    {
        logger.error("lparent missing in w_stmt");
        return -1;
    }
    get_word();
    if (word.first == "STRCON")
    {
        get_word();
        if (word.first == "COMMA")
        {
            get_word();
            exp();
        }
    }
    else
    {
        exp();
    }
    if (word.first == "RPARENT")
    {
        logger.error("rparent missing in w_stmt");
        return -1;
    }
    get_word();
    print_grammar("<写语句>");
    return 0;
}
/**
 * 返回语句
 * <ret_stmt> ::= return['('<exp>')']
 */
int GrammarTranslator::ret_stmt()
{
    if (word.first != "RETURNTK")
    {
        logger.error("returntk missing in ret_stmt");
        return -1;
    }
    get_word();
    if (word.first == "LPARENT")
    {
        get_word();
        exp();
        if (word.first != "RPARENT")
        {
            logger.error("rparent missing in ret_stmt with parentheses");
            return -1;
        }
        get_word();
    }
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
        logger.error("unknown \"%s\"", word.second.c_str());
        return -1;
    }
    words.close();
    fout.close();
    return 0;
}

#endif