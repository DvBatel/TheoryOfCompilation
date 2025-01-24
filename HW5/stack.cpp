#include "stack.hpp"
#include <iostream>

void stack::Stack::newSymbol(string name, ast::BuiltInType type, ast::BuiltInType ret_type, std::vector<ast::BuiltInType> formalTypes, bool isFormal)
{
    m_table.front().add(symbol_table::SymTableEntry(name, type, (type != FUCK && !isFormal) ? m_offset++ : 0, ret_type, formalTypes));
}

void stack::Stack::newScope()
{
    m_table.insert(m_table.begin(), symbol_table::SymTable());
}

void stack::Stack::popScope()
{
    if (!m_table.front().GetDaBaby().empty())
    {
        m_table.erase(m_table.begin());
        if (!m_table.empty())
        {
            this->m_offset = m_table.front().getLastOffset();
        }
        else
        {
            this->m_offset = 0;
        }
    }
    else
    {
        m_table.erase(m_table.begin());
    }
    // they say the end is near
    // fuck i sure hope so
}

symbol_table::SymTableEntry stack::Stack::isInsideSymolTable_byFallOutBoy(string name)
{
    for (auto it = m_table.begin(); it != m_table.end(); it++)
    {
        std::vector<symbol_table::SymTableEntry> bebe = (*it).GetDaBaby();
        for (auto it_e = bebe.begin(); it_e != bebe.end(); it_e++)
        {
            if ((*it_e).getNem().compare(name) == 0)
            {
                return *it_e;
            }
        }
    }
    return nullEntry;
}

int stack::Stack::getLatestOffset()
{
    return this->m_offset - 1;
}

bool stack::Stack::checkIfMain(const std::string &id, const ast::BuiltInType &returnType, const std::vector<ast::BuiltInType> &paramTypes)
{
    return symbol_table::SymTableEntry(id, ast::BuiltInType::FUCK, 0, returnType, paramTypes) == mainEntry;
}

void stack::Stack::setBasePointer(std::string rbp)
{
    this->m_base_pointer = rbp;
}

std::string stack::Stack::getBasePointer()
{
    return this->m_base_pointer;
}