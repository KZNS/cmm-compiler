#ifndef SYMBOL_TABLE_CPP
#define SYMBOL_TABLE_CPP

#include "symbol_table.h"

VarProperty::VarProperty() {}
VarProperty::VarProperty(const std::string &_name,
                         const std::string &_type,
                         bool _is_const)
    : name(_name), type(_type), is_const(_is_const) {}

FunctionProperty::FunctionProperty() {}
FunctionProperty::FunctionProperty(const std::string &_name,
                                   const std::string &_type,
                                   const std::vector<VarProperty> &_arg_list)
    : name(_name), type(_type), arg_list(_arg_list) {}

SymbolTable::SymbolTable() : is_local(false) {}
int SymbolTable::insert_var(const std::string &name,
                            const std::string &type, bool is_const)
{
    if (is_local)
    {
        if (local_var_table.find(name) != global_var_table.end())
        {
            logger.error("var %s already exists", name.c_str());
            return -1;
        }
        local_var_table[name] = VarProperty(name, type, is_const);
    }
    else
    {
        if (global_var_table.find(name) != global_var_table.end())
        {
            logger.error("var %s already exists", name.c_str());
            return -1;
        }
        global_var_table[name] = VarProperty(name, type, is_const);
    }
    return 0;
}
int SymbolTable::insert_f(const std::string &name, const std::string &type,
                          const std::vector<VarProperty> &arg_list)
{
    if (f_table.find(name) != f_table.end())
    {
        logger.error("function %s already exists", name.c_str());
        return -1;
    }
    f_table[name] = FunctionProperty(name, type, arg_list);
    return 0;
}

VarProperty *SymbolTable::find_var(const std::string &name)
{
    if (local_var_table.find(name) != local_var_table.end())
    {
        return &local_var_table[name];
    }
    else if (global_var_table.find(name) != global_var_table.end())
    {
        return &global_var_table[name];
    }
    else
    {
        return NULL;
    }
}
FunctionProperty *SymbolTable::find_f(const std::string &name)
{
    if (f_table.find(name) != f_table.end())
    {
        return &f_table[name];
    }
    else
    {
        return NULL;
    }
}

int SymbolTable::local_var_table_clear()
{
    local_var_table.clear();
    return 0;
}
int SymbolTable::set_local()
{
    is_local = true;
    return 0;
}
int SymbolTable::set_global()
{
    is_local = false;
    local_var_table_clear();
    return 0;
}

#endif