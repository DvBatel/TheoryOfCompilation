#include "symbolTable.hpp"

// this is good practice to have hpp file
symbol_table::SymTableEntry::SymTableEntry(std::string name, ast::BuiltInType type, int offset, ast::BuiltInType ret_type, std::vector<ast::BuiltInType> formalTypes) : m_name(name), m_type(type), m_offset(offset), m_ret_type(ret_type),m_formalTypes(formalTypes)
{
}
