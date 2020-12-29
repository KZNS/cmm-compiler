#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <map>
#include <string>
#include <utility>
#include <fstream>
#include <vector>
#include <queue>
#include "cLogger/clogger.h"

#define CHAR_NUM 95

extern Logger logger;

class AutomatonNode
{
private:
public:
    std::string type;
    AutomatonNode *next[CHAR_NUM];
    AutomatonNode();
    int link_to(const std::string &s, AutomatonNode *t);
};

typedef std::pair<std::string, std::string> Word;

class LexicalAutomaton
{
private:
    std::istream *in_source_code;
    AutomatonNode *root;
    std::vector<AutomatonNode *> node_list;
    char last;
    bool inited;
    int line_number;
    bool using_new_stream;

    std::string expand_rel(const std::string &rel);
    int split_str(std::string s, std::vector<std::string> &result);
    int link_rule(std::queue<AutomatonNode *> &ls, std::queue<AutomatonNode *> &nextls,
                  const std::string &rerule, const std::string &nextrule,
                  AutomatonNode *s, AutomatonNode *t, const std::string &type);
    int append_keyword_IDENFR(const std::string &rel, const std::string &type);
    int append_keyword_INTCON(const std::string &rel, const std::string &type);
    int append_keyword_CHARCON(const std::string &rel, const std::string &type);
    int append_keyword_STRCON(const std::string &rel, const std::string &type);

public:
    LexicalAutomaton();
    ~LexicalAutomaton();
    int open(std::istream &in_stream);
    int open(const std::string &file_name);
    int close();
    int append_keyword(const std::string &word, const std::string &type);
    Word get_word(int &new_line);
    bool empty();
};

#include "automaton.cpp"

#endif