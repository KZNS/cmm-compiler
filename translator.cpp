#ifndef TRANSLATOR_CPP
#define TRANSLATOR_CPP

#include "translator.h"
#include <string>
#include <fstream>

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

    words.close();
    return 0;
}
#endif