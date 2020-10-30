#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <map>
#include <string>

#define CHAR_NUM

class AutomatonNode
{
private:
public:
    string type;
    AutomatonNode *next[CHAR_NUM];
    AutomatonNode();
};

class LexicalAutomaton
{
private:
    std::ifstream file;
    AutomatonNode *root;

public:
    LexicalAutomaton();
    ~LexicalAutomaton();
    int open(const std::string &file_name);
    int append_keywords(const std::string &word, const std::string &type);
    pair<string, string> get_word();
};

#include "automaton.cpp"

#endif