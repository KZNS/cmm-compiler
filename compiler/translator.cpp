#ifndef TRANSLATOR_CPP
#define TRANSLATOR_CPP

#include "translator.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

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
    // [<declare_const>][<declare_var>]
    if (word.first == "CONSTTK")
    {
        declare_const();
    }
    if ((detect(2, "INTTK", "IDENFR") && !detect(3, "INTTK", "IDENFR", "LPARENT")) ||
        (detect(2, "CHARTK", "IDENFR") && !detect(3, "CHARTK", "IDENFR", "LPARENT")))
    {
        declare_var();
    }
    print_pcode("$main");
    print_pcode("exit 0");

    // {<f_ret>|<f_void>}<main_f>
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
    std::string type;
    std::string name;
    int x;
    char c;
    int e;

    // int
    // char
    if (word.first == "INTTK" || word.first == "CHARTK")
    {
        type = word.first;
        get_word();
    }
    else
    {
        logger.error("wrong type %s", word.second.c_str());
        return -1;
    }

    // <ident>=<integer>{,<ident>=<integer>}
    // <ident>=<ch>{,<ident>=<ch>}
    while (true)
    {
        if (word.first == "IDENFR")
        {
            name = word.second;
            get_word();
        }
        else
        {
            logger.error("missing identifier after %s", type.c_str());
            return -1;
        }
        //new var
        e = table.insert_var(VarProperty(name, type, true));
        if (!e)
        {
            print_pcode("var %s:%s", name.c_str(), type.c_str());
        }
        else
        {
            e_redifine_identifier();
        }

        if (word.first == "ASSIGN")
        {
            get_word();
        }
        else
        {
            logger.error("missing '=' after %s", name.c_str());
            return -1;
        }

        if (word.first == "PLUS" || word.first == "MINU" ||
            word.first == "INTCON" || word.first == "CHARCON")
        {
            if (type == "INTTK")
            {
                integer(x);
                print_pcode("push %d", x);
                print_pcode("pop %s", name.c_str());
            }
            else if (type == "CHARTK")
            {
                c = word.second[0];
                get_word();
                print_pcode("push %d", (int)c);
                print_pcode("pop %s", name.c_str());
            }
            else
            {
                logger.error("wrong type");
                return -1;
            }
        }
        else
        {
            e_const_define_type();
            get_word();
        }

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
    while ((detect(2, "INTTK", "IDENFR") && !detect(3, "INTTK", "IDENFR", "LPARENT")) ||
           (detect(2, "CHARTK", "IDENFR") && !detect(3, "CHARTK", "IDENFR", "LPARENT")))
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
    std::string type;
    std::string name;
    unsigned int size;
    int e;
    if (word.first == "INTTK" || word.first == "CHARTK")
    {
        type = word.first;
        get_word();
    }
    else
    {
        logger.error("wrong type %s", word.second.c_str());
        return -1;
    }

    while (true)
    {
        if (word.first == "IDENFR")
        {
            name = word.second;
            get_word();
        }
        else
        {
            logger.error("missing identifier after %s", type.c_str());
            return -1;
        }

        if (word.first == "LBRACK")
        {
            get_word();
            e = uinteger(size);
            if (e)
            {
                return -1;
            }
            if (word.first == "RBRACK")
            {
                get_word();
            }
            else
            {
                e_right_bracket();
            }
            e = table.insert_var(VarProperty(name, size, type));
            if (!e)
            {
                print_pcode("var %s[%d]:%s", name.c_str(), size, type.c_str());
            }
            else
            {
                e_redifine_identifier();
            }
        }
        else
        {
            e = table.insert_var(VarProperty(name, type));
            if (!e)
            {
                print_pcode("var %s:%s", name.c_str(), type.c_str());
            }
            else
            {
                e_redifine_identifier();
            }
        }

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
    else
    {
        x = 1;
    }

    unsigned int ux;
    int e;
    e = uinteger(ux);
    if (e)
    {
        return -1;
    }
    x *= ux;

    print_grammar("<整数>");
    return 0;
}

/**
 * 声明头部
 * <declare_h> ::= int<ident>
 *                 char<ident>
 */
int GrammarTranslator::declare_h(std::string &type, std::string &name)
{
    if (word.first == "INTTK" || word.first == "CHARTK")
    {
        type = word.first;
        get_word();

        if (word.first == "IDENFR")
        {
            name = word.second;
            get_word();
        }
        else
        {
            logger.error("missing function name");
            return -1;
        }
    }
    else
    {
        logger.error("missing declare_head");
        return -1;
    }

    print_grammar("<声明头部>");
    return 0;
}
/**
 * 有返回值函数定义
 * <f_ret> ::= <declare_h>'('<param_table>')''{'<comp_stmt>'}'
 */
int GrammarTranslator::f_ret()
{
    std::string type;
    std::string name;
    std::vector<VarProperty> arg_list;
    int e;

    table.set_local();

    // <declare_h>
    e = declare_h(type, name);
    if (e)
    {
        return -1;
    }
    print_pcode("FUNC @%s:", name.c_str());
    change_pcode_indent_deep(1);

    // '('<param_table>')'
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("missing '('");
        return -1;
    }
    param_table(arg_list);
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }

    table.insert_f(FunctionProperty(name, type, arg_list));

    // '{'<comp_stmt>'}'
    if (word.first == "LBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("missing '{'");
        return -1;
    }
    comp_stmt();
    if (word.first == "RBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("missing '}'");
        return -1;
    }
    change_pcode_indent_deep(-1);
    print_pcode("ENDFUNC");
    table.set_global();

    print_grammar("<有返回值函数定义>");
    return 0;
}
/**
 * 无返回值函数定义
 * <f_void> ::= void<ident>'('<param_table>')''{'<comp_stmt>'}'
 */
int GrammarTranslator::f_void()
{
    std::string name;
    std::string type;
    std::vector<VarProperty> arg_list;

    table.set_local();
    // void<ident>
    if (word.first == "VOIDTK")
    {
        type = word.first;
        get_word();
    }
    else
    {
        return -1;
    }
    if (word.first == "IDENFR")
    {
        name = word.second;
        get_word();
    }
    else
    {
        logger.error("missing function name");
        return -1;
    }
    print_pcode("FUNC @%s:", name.c_str());
    change_pcode_indent_deep(1);

    // '('<param_table>')'
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("missing '('");
        return -1;
    }
    param_table(arg_list);
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }

    table.insert_f(FunctionProperty(name, type, arg_list));

    // '{'<comp_stmt>'}'
    if (word.first == "LBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("missing '{'");
        return -1;
    }
    comp_stmt();
    if (word.first == "RBRACE")
    {
        get_word();
    }
    else
    {
        logger.error("missing '}'");
        return -1;
    }
    change_pcode_indent_deep(-1);
    print_pcode("ENDFUNC");
    table.set_global();

    print_grammar("<无返回值函数定义>");
    return 0;
}
/**
 * 参数表
 * <param_table> ::= <type><ident>{,<type><ident>}
 *                   <空>
 */
int GrammarTranslator::param_table(std::vector<VarProperty> &arg_list)
{
    std::string param_type;
    std::string param_name;
    int e;

    while (word.first == "INTTK" || word.first == "CHARTK")
    {
        param_type = word.first;
        get_word();

        if (word.first == "IDENFR")
        {
            param_name = word.second;
            get_word();
        }
        else
        {
            logger.error("missing param_name");
            return -1;
        }

        arg_list.push_back(VarProperty(param_name, param_type));
        e = table.insert_var(VarProperty(param_name, param_type));
        if (e)
        {
            e_redifine_identifier();
        }

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
    if (arg_list.size())
    {
        std::ostringstream tmp;
        tmp << "arg " << arg_list[0].name << ":" << arg_list[0].type;
        for (int i = 1; i < (int)arg_list.size(); i++)
        {
            tmp << "," << arg_list[i].name << ":" << arg_list[i].type;
        }
        print_pcode(tmp.str().c_str());
        print_pcode("");
    }
    else
    {
        print_pcode("arg");
        print_pcode("");
    }

    print_grammar("<参数表>");
    return 0;
}

/**
 * 主函数
 * <main_f> ::= void main'('')''{'<comp_stmt>'}'
 */
int GrammarTranslator::main_f()
{
    std::string name;
    std::string type;

    table.set_local();
    // void main'('')'
    if (word.first == "VOIDTK")
    {
        type = word.first;
        get_word();
    }
    else
    {
        logger.error("VOIDTK missing in main_f");
        return -1;
    }
    if (word.first == "MAINTK")
    {
        name = word.second;
        get_word();
    }
    else
    {
        logger.error("MAINTK missing in main_f");
        return -1;
    }

    print_pcode("FUNC @%s:", name.c_str());
    change_pcode_indent_deep(1);

    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("LPARENT missing in main_f");
        return -1;
    }
    print_pcode("arg");
    print_pcode("");
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }

    // '{'<comp_stmt>'}'
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
    change_pcode_indent_deep(-1);
    print_pcode("ENDFUNC");
    table.set_global();

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
    if ((detect(2, "INTTK", "IDENFR") && !detect(3, "INTTK", "IDENFR", "LPARENT")) ||
        (detect(2, "CHARTK", "IDENFR") && !detect(3, "CHARTK", "IDENFR", "LPARENT")))
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
    {
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
    if (word.first == "IFTK") // <cond_stmt>
    {
        cond_stmt();
    }
    else if (word.first == "DOTK" || word.first == "WHILETK" || word.first == "FORTK") // <loop_stmt>
    {
        loop_stmt();
    }
    else if (word.first == "LBRACE") // '{'<stmt_list>'}'
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
    else // statement with ';'
    {
        if (detect(2, "IDENFR", "LPARENT")) // <f_ret_call> | <f_void_call>
        {
            FunctionProperty *fp;
            fp = table.find_f(word.second);
            if (fp == NULL)
            {
                e_undifine_identifier();
            }
            else if (fp->type == "VOIDTK")
            {
                f_void_call();
            }
            else if (fp->type == "INTTK" || fp->type == "CHARTK")
            {
                f_ret_call();
                print_pcode("pop");
            }
            else
            {
                logger.error("unknow func type %s", fp->type.c_str());
            }
        }
        else if (word.first == "IDENFR") // <eval>
        {
            eval();
        }
        else if (word.first == "SCANFTK") // <r_stmt>
        {
            r_stmt();
        }
        else if (word.first == "PRINTFTK") // <w_stmt>
        {
            w_stmt();
        }
        else if (word.first == "RETURNTK") // <ret_stmt>
        {
            ret_stmt();
        }
        else if (word.first == "SEMICN") // <空>
        {
            ;
        }
        else
        {
            logger.error("invalid syntax in stmt");
            return -1;
        }

        if (word.first == "SEMICN") // ;
        {
            get_word();
        }
        else
        {
            e_semicolon();
        }
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
    std::string name;
    bool is_array;
    VarProperty *vp;
    std::string exp_type;

    // <ident> | <ident>'['<exp>']'
    if (word.first == "IDENFR")
    {
        name = word.second;
        get_word();
        vp = table.find_var(name);
        if (vp == NULL)
        {
            e_undifine_identifier();
        }
    }
    else
    {
        logger.error("missing idenfer in eval");
        return -1;
    }
    if (word.first == "LBRACK")
    {
        get_word();
        exp(exp_type);
        if (word.first == "RBRACK")
        {
            get_word();
        }
        else
        {
            e_right_bracket();
        }
        is_array = true;
        if (vp && !vp->is_array())
        {
            logger.error("%s is not an array", name.c_str());
            return -1;
        }
        if (exp_type != "INTTK")
        {
            e_array_index();
        }
    }
    else
    {
        is_array = false;
        if (vp && vp->is_array())
        {
            logger.error("%s is an array", name.c_str());
            return -1;
        }
    }

    // =<exp>
    if (word.first == "ASSIGN")
    {
        get_word();
    }
    else
    {
        logger.error("missing assign in eval");
        return -1;
    }
    exp(exp_type);
    if (vp && vp->type != exp_type)
    {
        logger.warn("different type");
    }

    if (is_array)
    {
        print_pcode("pop %s[]", name.c_str());
    }
    else
    {
        print_pcode("pop %s", name.c_str());
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
    // if'('<cond>')'<stmt>
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
        e_right_parenthesis();
    }
    stmt();

    // [else<stmt>]
    if (word.first == "ELSETK")
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
    std::string exp_type;
    std::string exp_type_b;
    exp(exp_type);
    if (word.first == "LSS" || word.first == "LEQ" || word.first == "GRE" ||
        word.first == "GEQ" || word.first == "EQL" || word.first == "NEQ")
    {
        get_word(); //rel_op();
        exp(exp_type_b);
    }
    if (exp_type != "INTTK" || exp_type_b != "INTTK")
    {
        e_condition_type();
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
    std::string exp_type;

    if (word.first == "WHILETK") // while'('<cond>')'<stmt>
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
            e_right_parenthesis();
        }
        stmt();
    }
    else if (word.first == "DOTK") // do<stmt>while'('<cond>')'
    {
        get_word();
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
            e_right_parenthesis();
        }
    }
    else if (word.first == "FORTK") // for'('<ident>=<exp>;<cond>;<ident>=<ident>(+|-)<step>')'<stmt>
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
        // <ident>=<exp>;
        if (word.first == "IDENFR")
        {
            get_word();
        }
        else
        {
            logger.error("idenfer missing in part1 of for in loop_stmt");
            return -1;
        }
        if (word.first == "ASSIGN")
        {
            get_word();
        }
        else
        {
            logger.error("assign missing in part1 of for in loop_stmt");
            return -1;
        }
        exp(exp_type);
        if (word.first == "SEMICN")
        {
            get_word();
        }
        else
        {
            logger.error("semicn missing in part1 of for in loop_stmt");
        }
        // <cond>;
        cond();
        if (word.first == "SEMICN")
        {
            get_word();
        }
        else
        {
            e_semicolon();
        }
        //<ident>=<ident>(+|-)<step>
        if (word.first == "IDENFR")
        {
            get_word();
        }
        else
        {
            logger.error("idenfer missing in part3 of for in loop_stmt");
            return -1;
        }
        if (word.first == "ASSIGN")
        {
            get_word();
        }
        else
        {
            logger.error("assign missing in part3 of for in loop_stmt");
            return -1;
        }
        if (word.first == "IDENFR")
        {
            get_word();
        }
        else
        {
            logger.error("idenfer missing in part3 of for in loop_stmt");
            return -1;
        }
        if (word.first == "PLUS" || word.first == "MINU")
        {
            get_word();
        }
        else
        {
            logger.error("(+|-) missing in part3 of for in loop_stmt");
            return -1;
        }
        step();
        // ')'<stmt>
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
int GrammarTranslator::exp(std::string &type)
{
    std::string op;
    std::string exp_type;
    std::string exp_type_b;

    // [+|-]<term>
    if (word.first == "PLUS" || word.first == "MINU")
    {
        op = word.first;
        get_word();
    }
    term(exp_type);
    if (op == "MINU")
    {
        print_pcode("neg");
    }

    // {<add_op><term>}
    while (word.first == "PLUS" || word.first == "MINU")
    {
        op = word.first;
        get_word();
        term(exp_type_b);
        if (exp_type != exp_type_b)
        {
            logger.warn("different type");
            if (exp_type_b == "INTTK")
            {
                exp_type = exp_type_b;
            }
        }
        if (op == "PLUS")
        {
            print_pcode("add");
        }
        else if (op == "MINU")
        {
            print_pcode("sub");
        }
    }

    print_grammar("<表达式>");
    return 0;
}
/**
 * 项
 * <term> ::= <factor>{<mult_op><factor>}
 */
int GrammarTranslator::term(std::string &type)
{
    std::string op;
    std::string exp_type;
    std::string exp_type_b;

    factor(exp_type);
    while (word.first == "MULT" || word.first == "DIV")
    {
        op = word.first;
        get_word();
        factor(exp_type_b);
        if (exp_type != exp_type_b)
        {
            logger.warn("different type");
            if (exp_type_b == "INTTK")
            {
                exp_type = exp_type_b;
            }
        }
        if (op == "MULT")
        {
            print_pcode("mul");
        }
        else if (op == "DIV")
        {
            print_pcode("div");
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
int GrammarTranslator::factor(std::string &type) /////////////////////////////////////////////////////////////////////////////////////////////
{
    VarProperty *vp;
    std::string name;
    bool is_array;
    std::string exp_type;

    if (detect(2, "IDENFR", "LPARENT")) // <f_ret_call>
    {
        f_ret_call();
    }
    else if (word.first == "IDENFR") // <ident> | <ident>'['<exp>']'
    {
        name = word.second;
        get_word();
        vp = table.find_var(name);
        if (vp == NULL)
        {
            e_undifine_identifier();
        }
        if (word.first == "LBRACK")
        {
            get_word();
            exp(exp_type);
            if (word.first == "RBRACK")
            {
                get_word();
            }
            else
            {
                e_right_bracket();
            }
            if (vp && !vp->is_array())
            {
                logger.error("%s is not an array", name.c_str());
                return -1;
            }
            print_pcode("push %s[]", name.c_str());
        }
        else
        {
            if (vp && vp->is_array())
            {
                logger.error("%s is an array", name.c_str());
                return -1;
            }
            print_pcode("push %s", name.c_str());
        }
    }
    else if (word.first == "LPARENT") // '('<exp>')'
    {
        get_word();
        exp(exp_type);
        if (word.first == "RPARENT")
        {
            get_word();
        }
        else
        {
            e_right_parenthesis();
        }
    }
    else if (word.first == "PLUS" || word.first == "MINU" || word.first == "INTCON") // <integer>
    {
        int x;
        integer(x);
        print_pcode("push %d", x);
    }
    else if (word.first == "CHARCON") // <ch>
    {
        char c;
        c = word.second[0];
        get_word();
        print_pcode("push %d", (int)c);
    }
    else
    {
        logger.error("wrong factor %s", word.second.c_str());
        return -1;
    }

    print_grammar("<因子>");
    return 0;
}

/**
 * 有返回值函数调用语句
 * <f_ret_call> ::= <ident>'('<arg_list>')'
 */
int GrammarTranslator::f_ret_call()
{
    FunctionProperty *fp;
    std::string name;
    std::vector<VarProperty> arg_list;

    if (word.first == "IDENFR")
    {
        name = word.second;
        get_word();
        fp = table.find_f(name);
        if (fp == NULL)
        {
            e_undifine_identifier();
        }
    }
    else
    {
        logger.error("ident missing in f_ret_call");
        return -1;
    }
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("lparent missing in f_ret_call");
        return -1;
    }
    GrammarTranslator::arg_list(arg_list);
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }
    if (fp)
    {
        if (fp->arg_list.size() != arg_list.size())
        {
            e_func_param_n();
        }
        else
        {
            for (int i = 0; i < arg_list.size(); i++)
            {
                if (fp->arg_list[i].type != arg_list[i].type)
                {
                    e_func_param_type();
                    break;
                }
            }
        }
    }
    print_pcode("$%s", name.c_str());

    print_grammar("<有返回值函数调用语句>");
    return 0;
}
/**
 * 无返回值函数调用语句
 * <f_void_call> ::= <ident>'('<arg_list>')'
 */
int GrammarTranslator::f_void_call()
{
    FunctionProperty *fp;
    std::string name;
    std::vector<VarProperty> arg_list;

    if (word.first == "IDENFR")
    {
        name = word.second;
        get_word();
        fp = table.find_f(name);
        if (fp == NULL)
        {
            e_undifine_identifier();
        }
    }
    else
    {
        logger.error("ident missing in f_void_call");
        return -1;
    }
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("lparent missing in f_void_call");
        return -1;
    }
    GrammarTranslator::arg_list(arg_list);
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }
    if (fp)
    {
        if (fp->arg_list.size() != arg_list.size())
        {
            e_func_param_n();
        }
        else
        {
            for (int i = 0; i < arg_list.size(); i++)
            {
                if (fp->arg_list[i].type != arg_list[i].type)
                {
                    e_func_param_type();
                    break;
                }
            }
        }
    }
    print_pcode("$%s", name.c_str());

    print_grammar("<无返回值函数调用语句>");
    return 0;
}
/**
 * 值参数表
 * <arg_list> ::= <exp>{,<exp>}
 *                <空>
 */
int GrammarTranslator::arg_list(std::vector<VarProperty> &arg_list)
{
    std::string exp_type;

    if (word.first == "PLUS" || word.first == "MINU" || word.first == "IDENFR" ||
        word.first == "LPARENT" || word.first == "INTCON" || word.first == "CHARCON")
    {
        while (true)
        {
            exp(exp_type);
            arg_list.push_back(VarProperty("", exp_type));
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
    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }

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
    std::string exp_type;
    if (word.first == "PRINTFTK")
    {
        get_word();
    }
    else
    {
        logger.error("printtk missing in w_stmt");
        return -1;
    }
    if (word.first == "LPARENT")
    {
        get_word();
    }
    else
    {
        logger.error("lparent missing in w_stmt");
        return -1;
    }

    // <str> | <str>,<exp> | <exp>
    if (word.first == "STRCON")
    {
        str_const();
        if (word.first == "COMMA")
        {
            get_word();
            exp(exp_type);
        }
    }
    else
    {
        exp(exp_type);
    }

    if (word.first == "RPARENT")
    {
        get_word();
    }
    else
    {
        e_right_parenthesis();
    }

    print_grammar("<写语句>");
    return 0;
}
/**
 * 返回语句
 * <ret_stmt> ::= return['('<exp>')']
 */
int GrammarTranslator::ret_stmt()
{
    std::string exp_type;
    if (word.first != "RETURNTK")
    {
        logger.error("returntk missing in ret_stmt");
        return -1;
    }
    get_word();
    if (word.first == "LPARENT")
    {
        get_word();
        exp(exp_type);
        if (word.first == "RPARENT")
        {
            get_word();
        }
        else
        {
            e_right_parenthesis();
        }
    }

    print_grammar("<返回语句>");
    return 0;
}

/**
 * 字符串
 * <str> ::= "{十进制编码为32,33,35-126的ASCII字符}"
 */
int GrammarTranslator::str_const()
{
    if (word.first == "STRCON")
    {
        get_word();
    }
    else
    {
        logger.error("wrong string %s", word.second.c_str());
        return -1;
    }

    print_grammar("<字符串>");
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

int GrammarTranslator::print_lexical(const Word &p_word)
{
    if (translate_type == "lexical" || translate_type == "grammar")
    {
        fout << word.first << " " << word.second << std::endl;
    }
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
int GrammarTranslator::print_pcode(std::string format, ...)
{
    if (translate_type == "PCODE")
    {
        char buffer[100];
        va_list args;

        va_start(args, format);
        vsnprintf(buffer, 100, format.c_str(), args);
        va_end(args);

        for (int i = 0; i < pcode_indent_deep; i++)
        {
            fout << "    ";
        }
        fout << buffer << std::endl;
    }
    return 0;
}

int GrammarTranslator::change_pcode_indent_deep(int x)
{
    pcode_indent_deep += x;
    if (pcode_indent_deep < 0)
    {
        pcode_indent_deep = 0;
    }
    return 0;
}

int GrammarTranslator::get_word()
{
    if (word.first != "")
    {
        print_lexical(word);
    }
    get_new_word();
    return 0;
}
int GrammarTranslator::get_new_word()
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
    if (now_word_id - step < bottom_word_id)
    {
        logger.error("roll back too much");
        return -1;
    }
    now_word_id -= step;
    word = word_buffer[now_word_id % WORD_BUFFER_SZ];
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
        get_new_word();
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
    pcode_indent_deep = 0;
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