#ifndef AUTOMATON_CPP
#define AUTOMATON_CPP

#include "automaton.h"
#include <fstream>

AutomatonNode::AutomatonNode()
{
    for (int i = 0; i < CHAR_NUM; i++)
        next[i] = NULL;
}
LexicalAutomaton::LexicalAutomaton()
{
    root = new AutomatonNode;
    //num
    AutomatonNode *t;
    t = new AutomatonNode;
    t->type = "<num>";
    for (char i = '0'; i <= '9'; i++)
    {
        root->next[i - ' '] = t;
        t->next[i-' '] = t;
    }
    //id
    t = new AutomatonNode;
    t->type = ""
}
int LexicalAutomaton::open(const std::string &file_name)
{
    file.open(file_name);
}
int LexicalAutomaton::append_keywords(const std::string &word, const std::string &type)
{
}
pair<string, string> get_word()
{
    char c;
    string s;
}

#endif