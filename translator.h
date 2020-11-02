#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "automaton.h"

class GrammarTranslator
{
private:
    LexicalAutomaton words;
    bool loaded_lex;

public:
    GrammarTranslator();
    int load_lexical(const std::string &file_name);
    int translate(const std::string &in_file_name, const std::string &out_file_name);
};

#include "translator.cpp"

#endif