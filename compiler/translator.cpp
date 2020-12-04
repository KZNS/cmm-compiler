#ifndef TRANSLATOR_CPP
#define TRANSLATOR_CPP

#include "translator.h"
#include <string>
#include <fstream>
#include <iostream>

GrammarTranslator::GrammarTranslator()
{
    loaded_lex = false;
}
int GrammarTranslator::load_lexical(const std::string &file_name)
{
    logger.debug("load lexical");
    if (loaded_lex)
    {
        logger.error("loaded lexical before");
        return -1;
    }
    std::ifstream file;
    file.open(file_name);

    logger.debug("loading keyword and type");
    std::string type, keyword;
    while (!loaded_lex && file >> type >> keyword)
    {
        if (type == "define" && keyword == "words")
        {
            while (file >> type >> keyword)
            {
                if (type == "define")
                {
                    break;
                    loaded_lex = true;
                }
                words.append_keyword(keyword, type);
            }
        }
    }
    file.close();

    words.init();
    return 0;
}
int GrammarTranslator::translate(const std::string &in_file_name,
                                 const std::string &out_file_name)
{
    words.open(in_file_name);
    fout.open(out_file_name);
    line_number = 0;
    int e;

    last_word = words.get_word();
    while (last_word.first != "")
    {
        e = S();
        if (e == -1)
        {
            logger.error("unknow \"%s\"", last_word.second);
            break;
        }
    }
    words.close();
    fout.close();
    return 0;
}

int GrammarTranslator::code()
{
    return 0;
}
int GrammarTranslator::codeblock()
{
    return 0;
}
int GrammarTranslator::sentences()
{
    return 0;
}

int GrammarTranslator::S()
{
    std::string &s = last_word.first;
    if (s == "<id>")
    {
    }
    else if (s == "<type_name>")
    {
    }
    else if (s == "<if>")
    {
    }
    else if (s == "<for>")
    {
    }
    else if (s == "<while>")
    {
    }
    else if (s == "<print>")
    {
    }
    else
    {
        return -1;
    }

    return 0;
}
int GrammarTranslator::equation()
{
    return 0;
}
int GrammarTranslator::branch_if()
{
    return 0;
}
int GrammarTranslator::branch_else()
{
    return 0;
}
int GrammarTranslator::branch_while()
{
    return 0;
}
int GrammarTranslator::difinition()
{
    return 0;
}

int GrammarTranslator::expression()
{
    return 0;
}

#endif