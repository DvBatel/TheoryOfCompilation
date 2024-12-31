#ifndef STACK_HPP
#define STACK_HPP

#include <string>
#include <vector>
#include "symbolTable.hpp"
#include "nodes.hpp"

using namespace std;
using namespace ast;

namespace stack {

    class Stack {
    private:
        std::vector<symbol_table::SymTable> m_table;
        int m_offset = 0;
    public:
      
    Stack() = default;
    ~Stack() = default;

    void addFormals(vector<shared_ptr<Formal>> formals);

    void newSymbol(string name, ast::BuiltInType type, ast::BuiltInType ret_type = ast::BuiltInType::FUCK, std::vector<ast::BuiltInType> formalTypes =  std::vector<ast::BuiltInType>());

    void newScope();

    void popScope();

    symbol_table::SymTableEntry isInsideSymolTable_byFallOutBoy(string name);

    symbol_table::SymTable Head ()
    {
        return this->m_table.front();
    }

    int getLatestOffset();

    bool checkIfMain(const std::string &id, const ast::BuiltInType &returnType, const std::vector<ast::BuiltInType> &paramTypes);
    };
}


#endif //STACK_HPP