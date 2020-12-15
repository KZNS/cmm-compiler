#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "cLogger/clogger.h"
#include <map>
#include <vector>
#include <string>

extern Logger logger;

class VarProperty
{
private:
public:
    VarProperty();
    VarProperty(const std::string &_name,
                const std::string &_type,
                bool _is_const);
    std::string name;
    std::string type;
    bool is_const;
};

class FunctionProperty
{
private:
public:
    FunctionProperty();
    FunctionProperty(const std::string &_name,
                     const std::string &_type,
                     const std::vector<VarProperty> &_arg_list);
    std::string name;
    std::string type;
    std::vector<VarProperty> arg_list;
};

class SymbolTable
{
private:
    std::map<std::string, VarProperty> global_var_table, local_var_table;
    std::map<std::string, FunctionProperty> f_table;
    bool is_local;

public:
    SymbolTable();
    int insert_var(const std::string &_name, const std::string &_type,
                   bool _is_const);
    int insert_f(const std::string &name, const std::string &type,
                 const std::vector<VarProperty> &arg_list);

    VarProperty *find_var(const std::string &name);
    FunctionProperty *find_f(const std::string &name);

    int local_var_table_clear();
    int set_local();
    int set_global();
};

#include "symbol_table.cpp"

#endif