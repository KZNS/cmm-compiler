#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <map>
#include <string>
#include <utility>
#include <fstream>
#include <vector>

#define CHAR_NUM 95

class AutomatonNode
{
private:
public:
    std::string type;
    AutomatonNode *next[CHAR_NUM];
    AutomatonNode();
};

class LexicalAutomaton
{
private:
    std::ifstream fin;
    AutomatonNode *root;
    std::vector<AutomatonNode*> node_list;
    char last;
    bool inited;

public:
    LexicalAutomaton();
    ~LexicalAutomaton();
    int open(const std::string &file_name);
    int close();
    int append_keyword(const std::string &word, const std::string &type);
    int init();
    std::pair<std::string, std::string> get_word();
    bool empty();
};

#include "automaton.cpp"

#endif