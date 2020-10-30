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
    std::ifstream file;
    AutomatonNode *root;
    std::vector<AutomatonNode*> node_list;
    char last;

public:
    LexicalAutomaton();
    ~LexicalAutomaton();
    int open(const std::string &file_name);
    int append_keywords(const std::string &word, const std::string &type);
    int init();
    std::pair<std::string, std::string> get_word();
};

#include "automaton.cpp"

#endif