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
int AutomatonNode::link_to(const std::string &s, AutomatonNode *t)
{
    for (int i = 0; i < s.length(); i++)
    {
        if (!next[s[i] - ' '])
            next[s[i] - ' '] = t;
    }
    return true;
}
std::string LexicalAutomaton::expand_rel(const std::string &rel)
{
    logger.debug("expand_rel %s", rel.c_str());
    using namespace std;

    bool range = false;
    char cu;
    string rule = "";
    for (int i = 0; i < rel.length(); i++)
    {
        if (rel[i] == '\\')
        {
            cu = (rel[i + 1] - '0') * 100 + (rel[i + 2] - '0') * 10 + rel[i + 3] - '0';
            i += 3;
        }
        else if (rel[i] == '~')
        {
            range = true;
            continue;
        }
        else
        {
            cu = rel[i];
        }

        if (range)
        {
            for (char k = rule[rule.length() - 1] + 1; k <= cu; k++)
            {
                rule += k;
            }
            range = false;
        }
        else
        {
            rule += cu;
        }
    }
    logger.debug("expand_rel to %s", rule.c_str());
    return rule;
}
int LexicalAutomaton::split_str(std::string s, std::vector<std::string> &result)
{
    int pos;
    while (pos = s.find('|'), pos != s.npos)
    {
        result.push_back(s.substr(0, pos));
        s = s.substr(pos + 1);
    }
    result.push_back(s);
    return result.size();
}
int LexicalAutomaton::link_rule(std::queue<AutomatonNode *> &ls, std::queue<AutomatonNode *> &nextls,
                                const std::string &rerule, const std::string &nextrule,
                                AutomatonNode *s, AutomatonNode *t, const std::string &type)
{
    logger.debug("link_rule");
    AutomatonNode *x;
    while (!ls.empty())
    {
        x = ls.front();
        ls.pop();

        if (x == s)
            continue;
        for (int i = 0; i < rerule.length(); i++)
        {
            AutomatonNode *&u = x->next[rerule[i] - ' '];
            if (u)
            {
                ls.push(u);
            }
            else
            {
                u = s;
            }
        }
        for (int i = 0; i < nextrule.length(); i++)
        {
            AutomatonNode *&u = x->next[nextrule[i] - ' '];
            if (u)
            {
                nextls.push(u);
                if (u->type == "")
                {
                    u->type = type;
                }
            }
            else
            {
                u = t;
            }
        }
    }
    logger.debug("link_rule done");
    return 0;
}

int LexicalAutomaton::append_keyword_IDENFR(const std::string &rel, const std::string &type)
{
    logger.debug("append keyword IDENFR %s by %s", type.c_str(), rel.c_str());
    using namespace std;
    vector<string> rules;
    split_str(rel, rules);
    for (int i = 0; i < rules.size(); i++)
    {
        rules[i] = expand_rel(rules[i]);
    }

    AutomatonNode *s, *t;
    s = new AutomatonNode;
    t = new AutomatonNode;
    s->link_to(rules[1], t);
    t->link_to(rules[1], t);
    s->type = type;
    t->type = type;

    // add in automation
    std::queue<AutomatonNode *> ls[2];
    ls[0].push(root);
    link_rule(ls[0], ls[1], "", rules[0], NULL, s, type);
    link_rule(ls[1], ls[0], rules[1], rules[1], t, t, type);

    return 0;
}
int LexicalAutomaton::append_keyword_INTCON(const std::string &rel, const std::string &type)
{
    logger.debug("append keyword INTCON %s by %s", type.c_str(), rel.c_str());
    using namespace std;
    vector<string> rules;
    split_str(rel, rules);
    for (int i = 0; i < rules.size(); i++)
    {
        rules[i] = expand_rel(rules[i]);
    }

    AutomatonNode *s;
    s = new AutomatonNode;
    s->link_to(rules[0], s);
    s->type = type;

    // add in automation
    std::queue<AutomatonNode *> ls[2];
    ls[0].push(root);
    link_rule(ls[0], ls[1], rules[0], rules[0], s, NULL, type);

    return 0;
}
int LexicalAutomaton::append_keyword_CHARCON(const std::string &rel, const std::string &type)
{
    logger.debug("append keyword CHARCON %s by %s", type.c_str(), rel.c_str());
    using namespace std;
    vector<string> rules;
    split_str(rel, rules);
    for (int i = 0; i < rules.size(); i++)
    {
        rules[i] = expand_rel(rules[i]);
    }
    AutomatonNode *s, *m, *t;
    s = new AutomatonNode;
    m = new AutomatonNode;
    t = new AutomatonNode;
    s->link_to(rules[1], m);
    m->link_to(rules[2], t);
    t->type = type;

    // add in automation
    std::queue<AutomatonNode *> ls[3];
    ls[0].push(root);
    link_rule(ls[0], ls[1], "", rules[0], NULL, s, "");
    link_rule(ls[1], ls[2], "", rules[1], NULL, m, "");
    link_rule(ls[2], ls[0], "", rules[2], NULL, t, type);

    return 0;
}
int LexicalAutomaton::append_keyword_STRCON(const std::string &rel, const std::string &type)
{
    logger.debug("append keyword STRCON %s by %s", type.c_str(), rel.c_str());
    using namespace std;
    vector<string> rules;
    split_str(rel, rules);
    for (int i = 0; i < rules.size(); i++)
    {
        rules[i] = expand_rel(rules[i]);
    }
    AutomatonNode *s, *m, *t;
    s = new AutomatonNode;
    m = new AutomatonNode;
    t = new AutomatonNode;
    s->link_to(rules[1], m);
    m->link_to(rules[1], m);
    m->link_to(rules[2], t);
    t->type = type;

    // add in automation
    std::queue<AutomatonNode *> ls[3];
    ls[0].push(root);
    link_rule(ls[0], ls[1], "", rules[0], NULL, s, "");
    link_rule(ls[1], ls[2], "", rules[1], NULL, m, "");
    link_rule(ls[2], ls[0], rules[1], rules[2], m, t, type);

    return 0;
}
LexicalAutomaton::LexicalAutomaton()
{
    root = new AutomatonNode;
    node_list.push_back(root);
    last = ' ' - 1;
    inited = false;
}
LexicalAutomaton::~LexicalAutomaton()
{
    if (fin.is_open())
        fin.close();
    for (int i = 0; i < node_list.size(); i++)
    {
        delete node_list[i];
    }
}
int LexicalAutomaton::open(const std::string &file_name)
{
    if (fin.is_open())
        fin.close();
    fin.open(file_name);
    return 0;
}
int LexicalAutomaton::close()
{
    if (fin.is_open())
        fin.close();
    return 0;
}
int LexicalAutomaton::append_keyword(const std::string &word, const std::string &type)
{
    if (inited)
        return -1;
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
    if (inited)
        return -1;
    inited = true;
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
        if (!fin.get(c))
        {
            c = -1;
            break;
        }
    }
    while (' ' <= c && c <= '~' && t->next[c - ' '] != NULL)
    {
        t = t->next[c - ' '];
        s += c;
        if (!fin.get(c))
        {
            c = -1;
            break;
        }
    }
    last = c;

    return make_pair(t->type, s);
}
bool LexicalAutomaton::empty()
{
    if (last == -1)
        return true;
    else
        return false;
}

#endif