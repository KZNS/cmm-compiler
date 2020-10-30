#ifndef AUTOMATON_CPP
#define AUTOMATON_CPP

#include "automaton.h"
#include <queue>
#include <utility>
#include <fstream>
#include <string>

AutomatonNode::AutomatonNode()
{
    for (int i = 0; i < CHAR_NUM; i++)
        next[i] = NULL;
}
LexicalAutomaton::LexicalAutomaton()
{
    root = new AutomatonNode;
    node_list.push_back(root);
    last = ' ' - 1;
}
LexicalAutomaton::~LexicalAutomaton()
{
    file.close();
    for (int i = 0; i < node_list.size(); i++)
    {
        delete node_list[i];
    }
}
int LexicalAutomaton::open(const std::string &file_name)
{
    file.open(file_name);
    return 0;
}
int LexicalAutomaton::append_keywords(const std::string &word, const std::string &type)
{
    AutomatonNode *t;
    char c;
    t = root;
    for (int i = 0; i < word.length(); i++)
    {
        c = word[i] - ' ';
        if (t->next[c] == NULL)
        {
            t->next[c] = new AutomatonNode;
            node_list.push_back(t->next[c]);
        }
        t = t->next[c];
    }
    t->type = type;
    return 0;
}
int LexicalAutomaton::init()
{
    AutomatonNode *t, *x;
    std::queue<AutomatonNode *> ls;
    //num
    t = new AutomatonNode;
    node_list.push_back(t);
    t->type = "<num>";
    for (int i = '0'; i <= '9'; i++)
    {
        t->next[i - ' '] = t;
        if (root->next[i - ' '])
        {
            ls.push(root->next[i - ' ']);
        }
        else
        {
            root->next[i - ' '] = t;
        }
    }
    while (!ls.empty())
    {
        x = ls.front();
        ls.pop();
        if (x == t)
            continue;
        if (x->type == "")
        {
            x->type = "<num>";
        }
        for (int i = '0'; i <= '9'; i++)
        {
            if (x->next[i - ' '])
            {
                ls.push(x->next[i - ' ']);
            }
            else
            {
                x->next[i - ' '] = t;
            }
        }
    }
    //id
    t = new AutomatonNode;
    node_list.push_back(t);
    t->type = "<id>";
    for (int i = 'a'; i <= 'z'; i++)
    {
        t->next[i - ' '] = t;
        if (root->next[i - ' '])
        {
            ls.push(root->next[i - ' ']);
        }
        else
        {
            root->next[i - ' '] = t;
        }
    }
    for (int i = 'A'; i <= 'Z'; i++)
    {
        t->next[i - ' '] = t;
        if (root->next[i - ' '])
        {
            ls.push(root->next[i - ' ']);
        }
        else
        {
            root->next[i - ' '] = t;
        }
    }
    t->next['_' - ' '] = t;
    if (root->next['_' - ' '])
    {
        ls.push(root->next['_' - ' ']);
    }
    else
    {
        root->next['_' - ' '] = t;
    }

    while (!ls.empty())
    {
        x = ls.front();
        ls.pop();
        if (x == t)
            continue;
        if (x->type == "")
        {
            x->type = "<id>";
        }
        for (int i = '0'; i <= '9'; i++)
        {
            if (x->next[i - ' '])
            {
                ls.push(x->next[i - ' ']);
            }
            else
            {
                x->next[i - ' '] = t;
            }
        }
        for (int i = 'a'; i <= 'z'; i++)
        {
            if (x->next[i - ' '])
            {
                ls.push(x->next[i - ' ']);
            }
            else
            {
                x->next[i - ' '] = t;
            }
        }
        for (int i = 'A'; i <= 'Z'; i++)
        {
            if (x->next[i - ' '])
            {
                ls.push(x->next[i - ' ']);
            }
            else
            {
                x->next[i - ' '] = t;
            }
        }
        if (x->next['_' - ' '])
        {
            ls.push(x->next['_' - ' ']);
        }
        else
        {
            x->next['_' - ' '] = t;
        }
    }
    return 0;
}
std::pair<std::string, std::string> LexicalAutomaton::get_word()
{
    if (last == 0)
    {
        return std::make_pair("", "");
    }
    char c;
    std::string s;
    AutomatonNode *t;
    c = last;
    t = root;
    while (!(' ' <= c && c <= '~' && t->next[c - ' '] != NULL))
    {
        if (!file.get(c))
        {
            c = 0;
            break;
        }
    }
    while (' ' <= c && c <= '~' && t->next[c - ' '] != NULL)
    {
        t = t->next[c - ' '];
        s += c;
        if (!(file.get(c)))
        {
            c = 0;
            break;
        }
    }
    last = c;

    return make_pair(t->type, s);
}

#endif