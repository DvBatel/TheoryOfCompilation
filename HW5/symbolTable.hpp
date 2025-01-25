#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <vector>
#include <iostream>
#include "nodes.hpp"

using namespace std;

namespace symbol_table
{
    class SymTableEntry
    {
    private:
        std::string m_name;
        ast::BuiltInType m_type; // if fuck then two other parameters can be used
        std::string m_var_emitted_name_by_falloutboy = "";
        int m_offset;

        // for function onlyyyyyy
        ast::BuiltInType m_ret_type;
        std::vector<ast::BuiltInType> m_formalTypes;

    public:
        // hey just fyi, if your rettype is FUCK then your are in fact NOT a func, and do not have a rettype at all
        // this is not confusing, don't worry about it
        SymTableEntry(string name, ast::BuiltInType type, int offset, ast::BuiltInType ret_type = ast::BuiltInType::FUCK, std::vector<ast::BuiltInType> formalTypes = std::vector<ast::BuiltInType>()) : m_name(name), m_type(type), m_offset(offset), m_ret_type(ret_type),m_formalTypes(formalTypes) {}
        
        std::string setEmittedName(std::string reg)
        {
            this->m_var_emitted_name_by_falloutboy = reg;
            return reg;
        }

        std::string getEmittedName()
        {
            return this->m_var_emitted_name_by_falloutboy;
        }
        
        ~SymTableEntry() = default;

        string getNem() { return this->m_name; }
        ast::BuiltInType getType() { return this->m_type; }
        ast::BuiltInType getRetType() { return this->m_ret_type; }
        int getOff() { return this->m_offset; }
        std::vector<ast::BuiltInType> getgetFormals() { return this->m_formalTypes; }
        // gotta get gotta get gotta get gotta get those formasl #jypapi

        bool operator==(symbol_table::SymTableEntry x)
        {
            if (x.m_name == this->m_name && x.m_type == this->m_type &&
                x.m_ret_type == this->m_ret_type && x.m_formalTypes == this->m_formalTypes)
            {
                return true;
            }
            return false;
        }

        bool operator!=(symbol_table::SymTableEntry x)
        {
            return !(this->operator==(x));
        }
        bool isFuck() { return this->m_type == ast::BuiltInType::FUCK; }
    };

    class SymTable
    {
    private:
        std::vector<SymTableEntry> m_baby;

    public:
        SymTable() = default;
        ~SymTable() = default;

        void add(SymTableEntry ste)
        {
            this->m_baby.push_back(ste);
        }

        SymTableEntry exist(string name)
        {
            for (auto it = m_baby.begin(); it != m_baby.end(); it++)
            {
                if (name.compare((*it).getNem()) == 0)
                {
                    return *it;
                }
            }
            return SymTableEntry("", ast::BuiltInType::FUCK, 0);
        }

        int getLastOffset()
        {
            int off = m_baby.back().getOff();
            if(!m_baby.empty())
                return max(off, 0);
            return 0;
        }

        std::vector<SymTableEntry>& GetDaBaby()
        {
            return m_baby;
        }
    };

}

static symbol_table::SymTableEntry nullEntry = symbol_table::SymTableEntry("", ast::BuiltInType::FUCK, 0);
static symbol_table::SymTableEntry mainEntry = symbol_table::SymTableEntry("main", ast::BuiltInType::FUCK, 0,ast::BuiltInType::VOID);
#endif 
// SYMBOLTABLE_HPP