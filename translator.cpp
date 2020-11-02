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
    if (loaded_lex)
        return -1;
    loaded_lex = true;
    std::ifstream file;
    file.open(file_name);

    std::string type, keyword;
    while (file >> type >> keyword)
    {
        words.append_keyword(keyword, type);
    }
    file.close();

    words.init();
    return 0;
}
int GrammarTanslator::translate(const std::string &in_file_name,
                                const std::string &out_file_name)
{
    words.open(in_file_name);
    fout.open(out_file_name);

    int e;

    last_word = words.get_word();
    while (last_word.first != "")
    {
        e = S();
        if (e == -1)
        {
            std::cout << "ERROR: " << last_word.second << std::endl;
            break;
        }
    }
    words.close();
    fout.close();
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
    else
    {
        return -1;
    }

    return 0;
}
#endif