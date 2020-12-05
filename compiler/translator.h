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
    std::pair<std::string, std::string> last_word;
    int line_number;

    int code();
    int codeblock();
    int sentences();
    int S();
    int equation();
    int branch_if();
    int branch_else();
    int branch_while();
    int difinition();

    int expression();

public:
    GrammarTranslator();
    int load_lexical(std::istream &file);
    int load_lexical(const std::string &file_name);
    int translate(const std::string &in_file_name, const std::string &out_file_name);
};

#include "translator.cpp"

#endif