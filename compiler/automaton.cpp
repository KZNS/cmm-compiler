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
    for (int i = 0; i < (int)s.length(); i++)
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
    for (int i = 0; i < (int)rel.length(); i++)
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
    while (pos = s.find('|'), pos != (int)std::string::npos)
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
        for (int i = 0; i < (int)rerule.length(); i++)
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
        for (int i = 0; i < (int)nextrule.length(); i++)
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
    for (int i = 0; i < (int)rules.size(); i++)
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
    for (int i = 0; i < (int)rules.size(); i++)
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
    for (int i = 0; i < (int)rules.size(); i++)
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
    for (int i = 0; i < (int)rules.size(); i++)
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
    in_source_code = NULL;
}
LexicalAutomaton::~LexicalAutomaton()
{
    close();
    for (int i = 0; i < (int)node_list.size(); i++)
    {
        delete node_list[i];
    }
}
int LexicalAutomaton::open(const std::string &file_name)
{
    if (in_source_code)
    {
        delete in_source_code;
    }
    in_source_code = new std::ifstream(file_name);
    line_number = 1;
    return 0;
}
int LexicalAutomaton::close()
{
    if (in_source_code)
    {
        delete in_source_code;
        in_source_code = NULL;
    }
    return 0;
}
int LexicalAutomaton::append_keyword(const std::string &word, const std::string &type)
{
    logger.debug("addend keyword %s as %s", word.c_str(), type.c_str());
    if (inited)
        return -1;
    if (type == "IDENFR")
    {
        append_keyword_IDENFR(word, type);
        return 0;
    }
    else if (type == "INTCON")
    {
        append_keyword_INTCON(word, type);
        return 0;
    }
    else if (type == "CHARCON")
    {
        append_keyword_CHARCON(word, type);
        return 0;
    }
    else if (type == "STRCON")
    {
        append_keyword_STRCON(word, type);
        return 0;
    }

    AutomatonNode *t;
    int c;
    t = root;
    for (int i = 0; i < (int)word.length(); i++)
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
Word LexicalAutomaton::get_word(int &new_line_number)
{
    if (last == -1)
    {
        return std::make_pair("-1", "");
    }
    char c;
    std::string s;
    AutomatonNode *t;
    c = last;
    t = root;
    while (!(' ' <= c && c <= '~' && t->next[c - ' '] != NULL))
    {
        if (c == '\n')
        {
            line_number++;
        }
        if (!in_source_code->get(c))
        {
            c = -1;
            break;
        }
    }
    while (' ' <= c && c <= '~' && t->next[c - ' '] != NULL)
    {
        t = t->next[c - ' '];
        s += c;
        if (!in_source_code->get(c))
        {
            c = -1;
            break;
        }
    }
    last = c;

    if (t->type == "CHARCON" || t->type == "STRCON")
    {
        s = s.substr(1, s.length() - 2);
    }

    new_line_number = line_number;
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