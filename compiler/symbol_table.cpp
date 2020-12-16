#ifndef SYMBOL_TABLE_CPP
#define SYMBOL_TABLE_CPP

#include "symbol_table.h"

VarProperty::VarProperty() {}
VarProperty::VarProperty(const std::string &_name,
                         const std::string &_type,
                         bool _is_const)
    : name(_name), size(0), type(_type), is_const(_is_const) {}
VarProperty::VarProperty(const std::string &_name,
                         int _size,
                         const std::string &_type)
    : name(_name), size(_size), type(_type), is_const(false) {}
bool VarProperty::is_array()
{
    return size;
}

FunctionProperty::FunctionProperty() {}
FunctionProperty::FunctionProperty(const std::string &_name,
                                   const std::string &_type,
                                   const std::vector<VarProperty> &_arg_list)
    : name(_name), type(_type), arg_list(_arg_list) {}

SymbolTable::SymbolTable() : is_local(false) {}
int SymbolTable::insert_var(const VarProperty &var_p)
{
    if (is_local)
    {
        if (local_var_table.find(var_p.name) != local_var_table.end())
        {
            logger.error("var %s already exists", var_p.name.c_str());
            return -1;
        }
        local_var_table[var_p.name] = var_p;
    }
    else
    {
        if (global_var_table.find(var_p.name) != global_var_table.end())
        {
            logger.error("var %s already exists", var_p.name.c_str());
            return -1;
        }
        global_var_table[var_p.name] = var_p;
    }
    return 0;
}
int SymbolTable::insert_f(const FunctionProperty &func_p)
{
    if (f_table.find(func_p.name) != f_table.end())
    {
        logger.error("function %s already exists", func_p.name.c_str());
        return -1;
    }
    f_table[func_p.name] = func_p;
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