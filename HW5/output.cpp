#include "output.hpp"
#include <iostream>
// literaly those two are only for one line
#include <algorithm>
#include <string>

using namespace generate;
namespace output
{
    const int MAX_BYTE = 255;
    int FORMAL = 1;
    const int DEBUG = 0;
    /* Helper functions */

    static std::string toString(ast::BuiltInType type)
    {
        switch (type)
        {
        case ast::BuiltInType::INT:
            return "int";
        case ast::BuiltInType::BOOL:
            return "bool";
        case ast::BuiltInType::BYTE:
            return "byte";
        case ast::BuiltInType::VOID:
            return "void";
        case ast::BuiltInType::STRING:
            return "string";
        case ast::BuiltInType::FUCK:
            return "fuck";
        default:
            return "unknown";
        }
    }

    /* Error handling functions */

    void errorLex(int lineno)
    {
        std::cout << "line " << lineno << ": lexical error\n";
        exit(0);
    }

    void errorSyn(int lineno)
    {
        std::cout << "line " << lineno << ": syntax error\n";
        exit(0);
    }

    void errorUndef(int lineno, const std::string &id)
    {
        std::cout << "line " << lineno << ":" << " variable " << id << " is not defined" << std::endl;
        exit(0);
    }

    void errorDefAsFunc(int lineno, const std::string &id)
    {
        std::cout << "line " << lineno << ":" << " symbol " << id << " is a function" << std::endl;
        exit(0);
    }

    void errorDefAsVar(int lineno, const std::string &id)
    {
        std::cout << "line " << lineno << ":" << " symbol " << id << " is a variable" << std::endl;
        exit(0);
    }

    void errorDef(int lineno, const std::string &id)
    {
        std::cout << "line " << lineno << ":" << " symbol " << id << " is already defined" << std::endl;
        exit(0);
    }

    void errorUndefFunc(int lineno, const std::string &id)
    {
        std::cout << "line " << lineno << ":" << " function " << id << " is not defined" << std::endl;
        exit(0);
    }

    void errorMismatch(int lineno)
    {
        std::cout << "line " << lineno << ":" << " type mismatch" << std::endl;
        exit(0);
    }

    void errorPrototypeMismatch(int lineno, const std::string &id, std::vector<std::string> &paramTypes)
    {
        std::cout << "line " << lineno << ": prototype mismatch, function " << id << " expects parameters (";

        for (int i = 0; i < paramTypes.size(); ++i)
        {
            // we transform the actuall array cus WE DONT GIVE A FUCK THIS IS ERROR WE DONE HERE GO FUCK YOURSELVES
            std::transform(paramTypes[i].begin(), paramTypes[i].end(), paramTypes[i].begin(), ::toupper);
            std::cout << paramTypes[i];
            if (i != paramTypes.size() - 1)
                std::cout << ",";
        }

        std::cout << ")" << std::endl;
        exit(0);
    }

    void errorUnexpectedBreak(int lineno)
    {
        std::cout << "line " << lineno << ":" << " unexpected break statement" << std::endl;
        exit(0);
    }

    void errorUnexpectedContinue(int lineno)
    {
        std::cout << "line " << lineno << ":" << " unexpected continue statement" << std::endl;
        exit(0);
    }

    void errorMainMissing()
    {
        std::cout << "Program has no 'void main()' function" << std::endl;
        exit(0);
    }

    void errorByteTooLarge(int lineno, const int value)
    {
        std::cout << "line " << lineno << ": byte value " << value << " out of range" << std::endl;
        exit(0);
    }

    /* ScopePrinter class */

    /*ScopePrinter::ScopePrinter() : indentLevel(0) {}

    std::string ScopePrinter::indent() const
    {
        std::string result;
        for (int i = 0; i < indentLevel; ++i)
        {
            result += "  ";
        }
        return result;
    }

    void ScopePrinter::beginScope()
    {
        indentLevel++;
        buffer << indent() << "---begin scope---" << std::endl;
    }

    void ScopePrinter::endScope()
    {
        buffer << indent() << "---end scope---" << std::endl;
        indentLevel--;
    }

    void ScopePrinter::emitVar(const std::string &id, const ast::BuiltInType &type, int offset)
    {
        buffer << indent() << id << " " << toString(type) << " " << offset << std::endl;
    }

    void ScopePrinter::emitFunc(const std::string &id, const ast::BuiltInType &returnType,
                                const std::vector<ast::BuiltInType> &paramTypes)
    {
        globalsBuffer << id << " " << "(";

        for (int i = 0; i < paramTypes.size(); ++i)
        {
            globalsBuffer << toString(paramTypes[i]);
            if (i != paramTypes.size() - 1)
                globalsBuffer << ",";
        }

        globalsBuffer << ")" << " -> " << toString(returnType) << std::endl;
    }

    std::ostream &operator<<(std::ostream &os, const ScopePrinter &printer)
    {
        os << "---begin global scope---" << std::endl;
        os << printer.globalsBuffer.str();
        os << printer.buffer.str();
        os << "---end global scope---" << std::endl;
        return os;
    }*/

    /* PrintVisitor implementation */

    PrintVisitor::PrintVisitor() : code()
    {
        if (DEBUG)
            cout << "entered initializer" << endl;
        // emit library functions
        this->stack.newScope();
        code.setStack(&this->stack);
        // this->printer.emitFunc("print", ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
        // this->printer.emitFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
        this->stack.newSymbol("print", ast::BuiltInType::FUCK, ast::BuiltInType::VOID, std::vector<ast::BuiltInType>(1, ast::BuiltInType::STRING));
        this->stack.newSymbol("printi", ast::BuiltInType::FUCK, ast::BuiltInType::VOID, std::vector<ast::BuiltInType>(1, ast::BuiltInType::INT));
    }

    void PrintVisitor::visit(ast::Num &node)
    {
        if (DEBUG)                         /*%%%*/
            cout << "entered int" << endl; /*%%%*/
        this->code.genNum(node);
        node.type_def   = toString(ast::BuiltInType::INT);
    }

    void PrintVisitor::visit(ast::NumB &node)
    {
        if (DEBUG)                          /*%%%*/
            cout << "entered numb" << endl; /*%%%*/
        
        if (node.value > MAX_BYTE)
        {
            errorByteTooLarge(node.line, node.value);
        }
        this->code.genNumB(node);
        node.type_def   = toString(ast::BuiltInType::BYTE);
    }

    void PrintVisitor::visit(ast::String &node)
    {
        if (DEBUG)                            /*%%%*/
            cout << "entered string" << endl; /*%%%*/
        this->code.genString(node);
        node.type_def = toString(ast::BuiltInType::STRING);
    }

    void PrintVisitor::visit(ast::Bool &node)
    {
        if (DEBUG)                          /*%%%*/
            cout << "entered bool" << endl; /*%%%*/
        node.type_def = toString(ast::BuiltInType::BOOL);
        this->code.genBool(node);
    }

    void PrintVisitor::visit(ast::ID &node)
    {
        // im in
        /*%%%*/ if (DEBUG)
            std::cout << "entered id" << endl; /*%%%*/
        node.type_def = node.value;
        this->code.genID(node);
        /*%%%*/ if (DEBUG)
            std::cout << "out of id" << endl; /*%%%*/
    }

    void PrintVisitor::assureNumber(ast::Exp &node)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.type_def);
        if (entry != nullEntry && entry.isFuck())
        {
            // in the symtab and is a fuck
            errorDefAsFunc(node.line, node.type_def);
        }
        else if (entry != nullEntry)
        {
            // in the symtab and is a variant
            // int foo () { return 4; } int main () { int x = 4 + foo(); }
            node.type_def = toString(entry.getType());
        }
        // not in tsymtab --->  number or undefined var  or mismatch
        if ((node.type_def.compare(toString(ast::BuiltInType::INT)) != 0) &&
            (node.type_def.compare(toString(ast::BuiltInType::BYTE)) != 0))
        {
            if ((node.type_def.compare(toString(ast::BuiltInType::BOOL)) == 0) || (node.type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                (node.type_def.compare(toString(ast::BuiltInType::VOID)) == 0) || (node.type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
            {
                errorMismatch(node.line);
            }
            errorUndef(node.line, node.type_def);
        }
    }

    void PrintVisitor::visit(ast::BinOp &node)
    {
        if (DEBUG)                           /*%%%*/
            cout << "entered binop" << endl; /*%%%*/

        // go inside recursively and return the values
        node.left->accept(*this);
        node.right->accept(*this);

        assureNumber(*(node.left));
        assureNumber(*(node.right));
        node.type_def = ((node.left->type_def.compare(toString(ast::BuiltInType::INT)) == 0) ||
                         (node.right->type_def.compare(toString(ast::BuiltInType::INT)) == 0))
                            ? toString(ast::BuiltInType::INT)
                            : toString(ast::BuiltInType::BYTE);
        this->code.genBinOp(node);
    }

    void PrintVisitor::visit(ast::RelOp &node)
    {
        if (DEBUG)                           /*%%%*/
            cout << "entered relop" << endl; /*%%%*/

        // go inside recursively and return the values
        node.left->accept(*this);
        node.right->accept(*this);

        assureNumber(*(node.left));
        assureNumber(*(node.right));
        
        node.type_def = toString(ast::BuiltInType::BOOL);
        this->code.genRelOp(node);
    }

    void PrintVisitor::visit(ast::Type &node)
    {
        if (DEBUG)                          /*%%%*/
            cout << "entered type" << endl; /*%%%*/
        node.type_def = toString(node.getType());
    }

    void PrintVisitor::visit(ast::Cast &node)
    {
        if (DEBUG)                          /*%%%*/
            cout << "entered cast" << endl; /*%%%*/
        node.target_type->accept(*this);    // the explicit cast type
        node.exp->accept(*this);            // the current value

        if (node.target_type->getType() != ast::BuiltInType::INT &&
            node.target_type->getType() != ast::BuiltInType::BYTE)
        {
            errorMismatch(node.target_type->line);
        }
        assureNumber(*(node.exp));
        node.type_def = node.target_type->type_def;
        this->code.genCast(node);
    }

    void PrintVisitor::assureBoolean(ast::Exp &node1, ast::Exp &node2)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node1.type_def);
        if (entry != nullEntry && ((entry.isFuck() && (entry.getRetType() != ast::BuiltInType::BOOL)) ||
                                   (!entry.isFuck() && (entry.getType() != ast::BuiltInType::BOOL))))
        {
            errorDefAsFunc(node1.line, node1.type_def);
        }
        else if (entry != nullEntry)
        {
            node1.type_def = toString(ast::BuiltInType::BOOL);
        }

        if (node1.type_def.compare(toString(ast::BuiltInType::BOOL)) != 0)
        {
            if ((node1.type_def.compare(toString(ast::BuiltInType::INT)) == 0) || (node1.type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                (node1.type_def.compare(toString(ast::BuiltInType::BYTE)) == 0) || (node1.type_def.compare(toString(ast::BuiltInType::VOID)) == 0) ||
                (node1.type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
            {
                errorMismatch(node1.line);
            }
            else
            {
                errorUndef(node1.line, node1.type_def);
            }
        }

        entry = this->stack.isInsideSymolTable_byFallOutBoy(node2.type_def);
        if (entry != nullEntry && ((entry.isFuck() && (entry.getRetType() != ast::BuiltInType::BOOL)) ||
                                   (!entry.isFuck() && (entry.getType() != ast::BuiltInType::BOOL))))
        {
            errorDefAsFunc(node2.line, node2.type_def);
        }
        else if (entry != nullEntry)
        {
            node2.type_def = toString(ast::BuiltInType::BOOL);
        }

        if (node2.type_def.compare(toString(ast::BuiltInType::BOOL)) != 0)
        {
            if ((node2.type_def.compare(toString(ast::BuiltInType::INT)) == 0) || (node2.type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                (node2.type_def.compare(toString(ast::BuiltInType::BYTE)) == 0) || (node2.type_def.compare(toString(ast::BuiltInType::VOID)) == 0) ||
                (node2.type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
            {
                errorMismatch(node2.line);
            }
            else
            {
                errorUndef(node2.line, node2.type_def);
            }
        }
    }

    void PrintVisitor::visit(ast::Not &node)
    {
        if (DEBUG)                         /*%%%*/
            cout << "entered not" << endl; /*%%%*/
        node.exp->accept(*this);

        assureBoolean(*(node.exp), *(node.exp));

        node.type_def = toString(ast::BuiltInType::BOOL);
        this->code.genNot(node);
    }

    void PrintVisitor::visit(ast::And &node)
    {
        if (DEBUG)                         /*%%%*/
            cout << "entered and" << endl; /*%%%*/
        node.left->accept(*this);
        this->code.genAndLeft(node);

        node.right->accept(*this);
        this->code.genAndRight(node);

        assureBoolean(*(node.left), *(node.right));

        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::Or &node)
    {
        if (DEBUG)                        /*%%%*/
            cout << "entered or" << endl; /*%%%*/
        node.left->accept(*this);
        this->code.genOrLeft(node);

        node.right->accept(*this);
        this->code.genOrRight(node);

        assureBoolean(*(node.left), *(node.right));

        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::ExpList &node)
    {
        if (DEBUG)                             /*%%%*/
            cout << "entered explist" << endl; /*%%%*/

        for (auto it = node.exps.rbegin(); it != node.exps.rend(); ++it)
        {
            (*it)->accept(*this);
        }
    }

    void PrintVisitor::visit(ast::Call &node)
    {
        if (DEBUG)                          /*%%%*/
            cout << "entered call" << endl; /*%%%*/

        node.func_id->accept(*this);
        node.args->accept(*this);

        std::vector<std::string> expectedArgs;
        std::vector<ast::BuiltInType> formals;
        symbol_table::SymTableEntry entry =
            this->stack.isInsideSymolTable_byFallOutBoy(node.func_id->type_def);
        if ((entry == nullEntry))
        {
            // not in symtab
            errorUndefFunc(node.func_id->line, node.func_id->type_def);
        }
        else if (!entry.isFuck())
        {
            // it is but it is a var
            errorDefAsVar(node.func_id->line, node.func_id->type_def);
        }
        // all good
        // the func is funcing
        formals = entry.getgetFormals();
        for (auto it = formals.begin(); it != formals.end(); ++it)
        {
            // it is vector
            expectedArgs.push_back(toString(*it));
        }

        // go through the explist and retrieve the types
        std::vector<std::shared_ptr<ast::Exp>> sentFormals = node.args->getExpList();
        auto formal = expectedArgs.begin();
        std::vector<std::shared_ptr<ast::Exp>>::iterator it;
        for (it = sentFormals.begin(); it != sentFormals.end() && formal != expectedArgs.end(); ++it, ++formal)
        {
            // we have to search for the exp type bc maybe its a variable :(
            symbol_table::SymTableEntry entryExp = this->stack.isInsideSymolTable_byFallOutBoy((*it)->type_def);
            if (entryExp != nullEntry)
            {
                (*it)->type_def = toString(entryExp.getType());
            }
            else if ((entryExp == nullEntry) && !(((*it)->type_def.compare(toString(ast::BuiltInType::BOOL)) == 0) ||
                                                  ((*it)->type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                                                  ((*it)->type_def.compare(toString(ast::BuiltInType::VOID)) == 0) ||
                                                  ((*it)->type_def.compare(toString(ast::BuiltInType::INT)) == 0) ||
                                                  ((*it)->type_def.compare(toString(ast::BuiltInType::BYTE)) == 0)))
            {
                errorUndef((*it)->line, (*it)->type_def);
            }
            // a: exp, b: inputed
            // !(a==b || (a== int && b == byte))
            if (((*it)->type_def.compare((*formal)) != 0) && ((*it)->type_def.compare("byte") != 0 || (*formal).compare("int") != 0))
            {
                // they different
                errorPrototypeMismatch((*it)->line, node.func_id->type_def, expectedArgs);
            }
        }
        if ((formal != expectedArgs.end()) || (it != sentFormals.end()))
        {
            errorPrototypeMismatch(node.args->line, node.func_id->type_def, expectedArgs);
        }
        // set the functions return type as it was set in the entry
        node.type_def = toString(entry.getRetType());
        this->code.genCall(node);
    }

    void PrintVisitor::visit(ast::Statements &node)
    {
        if (DEBUG)                                /*%%%*/
            cout << "entered statements" << endl; /*%%%*/
        if (node.isInBraces)
        {
            // this->printer.beginScope();
            this->stack.newScope();
            if (!node.getFormals().empty())
            {
                auto formals = node.getFormals();
                int offsetGeorge = -1;
                for (auto it = formals.begin(); it != formals.end(); ++it)
                {
                    symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy((*it)->id->getVal());
                    if (entry != nullEntry)
                    {
                        errorDef((*it)->id->line, (*it)->id->getVal());
                    }
                    this->stack.newFormal((*it)->id->value, (*it)->type->type, ast::BuiltInType::FUCK, std::vector<ast::BuiltInType>(), offsetGeorge--);
                    // this->printer.emitVar((*it)->id->value, (*it)->type->type, offsetGeorge--);
                }
            }
        }
        for (auto it = node.statements.begin(); it != node.statements.end(); ++it)
        {
            (*it)->accept(*this);
        }
        if (node.isInBraces)
        {
            // this->printer.endScope();
            this->stack.popScope();
        }
    }

    void PrintVisitor::visit(ast::Break &node)
    {
        if (DEBUG)                           /*%%%*/
            cout << "entered break" << endl; /*%%%*/
        if (inLoop <= 0)
        {
            errorUnexpectedBreak(node.line);
        }
        this->code.genBreak(node);
    }

    void PrintVisitor::visit(ast::Continue &node)
    {
        if (DEBUG)                              /*%%%*/
            cout << "entered continue" << endl; /*%%%*/
        if (inLoop <= 0)
        {
            errorUnexpectedContinue(node.line);
        }
        this->code.genContinue(node);
    }

    void PrintVisitor::visit(ast::Return &node)
    {
        if (DEBUG)                            /*%%%*/
            cout << "entered return" << endl; /*%%%*/

        if (node.exp)
        {
            node.exp->accept(*this);
            // born to party
            // forced to symbol table
            symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.exp->type_def);
            if (entry != nullEntry && entry.isFuck())
            {
                // in the symtab and is a fuck
                errorDefAsFunc(node.exp->line, node.exp->type_def);
            }
            else if (entry != nullEntry)
            {
                // in the symtab and is a variant
                // int foo () { return 4; } int main () { int x = 4 + foo(); }
                node.type_def = toString(entry.getType());
            }
            else
            {
                node.type_def = node.exp->type_def;
            }
        }
        else
        {
            node.type_def = toString(ast::BuiltInType::VOID);
        }
        this->code.genReturn(node);
    }

    bool PrintVisitor::isBoolean(ast::Exp &condition)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(condition.type_def);
        if (entry != nullEntry && ((entry.isFuck() && (entry.getRetType() != ast::BuiltInType::BOOL)) ||
                                   (!entry.isFuck() && (entry.getType() != ast::BuiltInType::BOOL))))
        {
            return false;
        }
        else if (entry != nullEntry)
        {
            condition.type_def = toString(ast::BuiltInType::BOOL);
        }
        else
        {
            errorUndef(condition.line, condition.type_def);
            return false;
        }

        if (condition.type_def.compare(toString(ast::BuiltInType::BOOL)) == 0)
        {
            return true;
        }
        return false;
    }

    void PrintVisitor::visit(ast::If &node)
    {
        if (DEBUG)                        /*%%%*/
            cout << "entered if" << endl; /*%%%*/

        node.condition->accept(*this);
        assureBoolean(*(node.condition), *(node.condition));
        this->code.genIfCondition(node);

        // this->printer.beginScope();
        node.then->accept(*this);
        this->code.genIfThen(node);
        // this->printer.endScope();

        if (node.otherwise)
        {
            // this->printer.beginScope();
            node.otherwise->accept(*this);
            this->code.genIfOtherwise(node);
            // this->printer.endScope();
        }
    }

    void PrintVisitor::visit(ast::While &node)
    {
        if (DEBUG)                           /*%%%*/
            cout << "entered while" << endl; /*%%%*/

        this->code.genWhileCondition(node);
        node.condition->accept(*this);
        assureBoolean(*(node.condition), *(node.condition));

        this->inLoop++;
        // this->printer.beginScope();
        this->code.genWhileBody(node);
        node.body->accept(*this);
        this->code.genWhileEnd(node);
        // this->printer.endScope();
        this->inLoop--;
    }

    void PrintVisitor::assureAssignCorrect(std::shared_ptr<ast::Exp> exp, ast::BuiltInType nodeType)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(exp->type_def);
        if (entry != nullEntry && entry.isFuck())
        {
            // in the symtab and is a fuck
            errorDefAsFunc(exp->line, exp->type_def);
        }
        else if (entry != nullEntry)
        {
            // in the symtab and is a variant
            // int foo () { return 4; } int main () { int x = 4 + foo(); }
            exp->type_def = toString(entry.getType());
        }

        switch (nodeType)
        {
        case ast::BuiltInType::INT:
            // secong can by int or byte
            if ((exp->type_def.compare(toString(ast::BuiltInType::INT)) != 0 &&
                 exp->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0))
            {
                if ((exp->type_def.compare(toString(ast::BuiltInType::BOOL)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                    (exp->type_def.compare(toString(ast::BuiltInType::VOID)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
                {
                    errorMismatch(exp->line);
                }
                errorUndef(exp->line, exp->type_def);
            }
            break;
        case ast::BuiltInType::BYTE:
            if (exp->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0)
            {
                if ((exp->type_def.compare(toString(ast::BuiltInType::INT)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::BOOL)) == 0) ||
                    (exp->type_def.compare(toString(ast::BuiltInType::STRING)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::VOID)) == 0) ||
                    (exp->type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
                {
                    errorMismatch(exp->line);
                }
                errorUndef(exp->line, exp->type_def);
            }
            // she's a certified freak
            // ten day a week
            break;
        case ast::BuiltInType::BOOL:
            // secong can by bool
            if (exp->type_def.compare(toString(ast::BuiltInType::BOOL)) != 0)
            {
                if ((exp->type_def.compare(toString(ast::BuiltInType::INT)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::STRING)) == 0) ||
                    (exp->type_def.compare(toString(ast::BuiltInType::VOID)) == 0) || (exp->type_def.compare(toString(ast::BuiltInType::BYTE)) == 0) ||
                    (exp->type_def.compare(toString(ast::BuiltInType::FUCK)) == 0))
                {
                    errorMismatch(exp->line);
                }
                errorUndef(exp->line, exp->type_def);
            }
            break;
        default:
            // mayhaps a string
            errorMismatch(exp->line);
            break;
        }
    }

    void PrintVisitor::visit(ast::VarDecl &node)
    {
        if (DEBUG)                             /*%%%*/
            cout << "entered vardecl" << endl; /*%%%*/

        node.id->accept(*this);
        node.type->accept(*this);
        bool initexpExists = false;
        if (node.init_exp)
        {
            node.init_exp->accept(*this);
            initexpExists = true;
        }

        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.id->getVal());
        if (entry != nullEntry)
        {
            errorDef(node.id->line, node.id->getVal());
        }

        if (initexpExists)
        {
            assureAssignCorrect(node.init_exp, node.type->getType());
        }
        this->stack.newSymbol(node.id->getVal(), node.type->getType());
        // the latest offset is the offset added to the symboltable
        // this->printer.emitVar(node.id->getVal(), node.type->getType(), this->stack.getLatestOffset());
        this->code.genVarDecl(node);
    }

    void PrintVisitor::visit(ast::Assign &node)
    {
        if (DEBUG)                            /*%%%*/
            cout << "entered assign" << endl; /*%%%*/

        node.id->accept(*this);
        node.exp->accept(*this);

        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.id->getVal());
        if (entry == nullEntry)
        {
            errorUndef(node.id->line, node.id->getVal());
        }
        else if (entry.isFuck())
        {
            // this is fuck
            errorDefAsFunc(node.id->line, node.id->getVal());
        }
        assureAssignCorrect(node.exp, entry.getType());
        this->code.genAssign(node);
    }

    void PrintVisitor::visit(ast::Formal &node)
    {
        if (DEBUG)                            /*%%%*/
            cout << "entered formal" << endl; /*%%%*/

        node.id->accept(*this);
        node.type->accept(*this);
    }

    void PrintVisitor::visit(ast::Formals &node)
    {
        if (DEBUG)                             /*%%%*/
            cout << "entered formals" << endl; /*%%%*/

        for (auto it = node.formals.rbegin(); it != node.formals.rend(); ++it)
        {
            (*it)->accept(*this);
        }
    }

    void PrintVisitor::visit(ast::FuncDecl &node)
    {
        if (DEBUG)                              /*%%%*/
            cout << "entered funcdecl" << endl; /*%%%*/
    
        std::vector<std::shared_ptr<ast::Formal>> formalsToBeAdded = node.getFormals();
        // check if the formal's names suits a function name
        node.id->accept(*this);
        node.return_type->accept(*this);
        this->code.genFuncDeclEntery(node);

        node.formals->accept(*this);
        node.body->accept(*this);

        // check if there are returns - need to make sure the types are correct
        std::vector<std::shared_ptr<ast::Statement>> statements = node.body->statements;
        for (auto it = statements.begin(); it != statements.end(); ++it)
        {
            if ((*it)->isReturn)
            {
                if (node.return_type->type_def.compare(toString(ast::BuiltInType::INT)) == 0)
                {
                    // check if int - then check if int or byte
                    if (((*it)->type_def.compare(toString(ast::BuiltInType::INT)) != 0) &&
                        ((*it)->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0))
                    {
                        errorMismatch((*it)->line);
                    }
                }
                // else - all the other types need to return the same type
                else if ((*it)->type_def.compare(node.return_type->type_def) != 0)
                {
                    errorMismatch(node.return_type->line);
                }
            }
        }
        this->code.genFuncDeclClosery(node);
    }

    void PrintVisitor::visit(ast::Funcs &node)
    {
        if (DEBUG)                           /*%%%*/
            cout << "entered funcs" << endl; /*%%%*/
        this->code.genFuncs(node);

        bool isThereMain = false;
        for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it)
        { // emit functions decls
            symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy((*it)->getID());
            if (entry != nullEntry)
            {
                errorDef((*it)->id->line, (*it)->getID());
            }
            // check if the funcdecl is main
            if (!isThereMain)
            {
                isThereMain = this->stack.checkIfMain((*it)->getID(), (*it)->getType(), (*it)->formalTypes());
            }
            // this->printer.emitFunc((*it)->getID(), (*it)->getType(), (*it)->formalTypes());
            this->stack.newSymbol((*it)->getID(), ast::BuiltInType::FUCK, (*it)->getType(), (*it)->formalTypes());
        }
        if (!isThereMain)
        {
            errorMainMissing();
        }
        // here we good
        for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it)
        { // transverse those bitches
            (*it)->getStatememts()->insertFormals((*it)->getFormals());
            (*it)->accept(*this); // the accepts just visits those bitches
        }
        this->stack.popScope();
    }

    void PrintVisitor::print()
    {
        // std::cout << this->printer; // prints the sexy beast that is called the program
        this->code.printCode(); // prints the sexy beast that is called llvm generated code
    }
}
