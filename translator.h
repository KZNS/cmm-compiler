#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "automaton.h"
#include <utility>

class GrammarTranslator
{
private:
    LexicalAutomaton words;
    bool loaded_lex;
    std::ofstream fout;
    std::pair<std::string, std::string> last_word;

    int S();

public:
    GrammarTranslator();
    int load_lexical(const std::string &file_name);
    int translate(const std::string &in_file_name, const std::string &out_file_name);
};

#include "translator.cpp"

#endif