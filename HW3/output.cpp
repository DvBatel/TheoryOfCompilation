#include "output.hpp"
#include <iostream>

namespace output
{
    const int MAX_BYTE = 255;
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

    ScopePrinter::ScopePrinter() : indentLevel(0) {}

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
    // SP:   cout << sp << endl;
    std::ostream &operator<<(std::ostream &os, const ScopePrinter &printer)
    {
        os << "---begin global scope---" << std::endl;
        os << printer.globalsBuffer.str();
        os << printer.buffer.str();
        os << "---end global scope---" << std::endl;
        return os;
    }

    /* PrintVisitor implementation */

    PrintVisitor::PrintVisitor() : printer()
    {
        if(DEBUG) cout << "entered initializer" << endl;
        // emit library functions
        this->stack.newScope();
        this->printer.emitFunc("print",  ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
        this->printer.emitFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
        this->stack.newSymbol ("print",  ast::BuiltInType::FUCK,  ast::BuiltInType::VOID, std::vector<ast::BuiltInType>(1, ast::BuiltInType::STRING));
        this->stack.newSymbol ("printi", ast::BuiltInType::FUCK,  ast::BuiltInType::VOID, std::vector<ast::BuiltInType>(1, ast::BuiltInType::INT));
    }

    void PrintVisitor::visit(ast::Num &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered int" << endl; /*%%%*/
        node.type_def = toString(ast::BuiltInType::INT);
    }

    void PrintVisitor::visit(ast::NumB &node)
    {
        if (node.value > MAX_BYTE)
        {
            errorByteTooLarge(node.line, node.value);
        }
        if(DEBUG) /*%%%*/ cout << "entered numb" << endl; /*%%%*/
        node.type_def = toString(ast::BuiltInType::BYTE);
    }

    void PrintVisitor::visit(ast::String &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered string" << endl; /*%%%*/
        node.type_def = toString(ast::BuiltInType::STRING);
    }

    void PrintVisitor::visit(ast::Bool &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered bool" << endl; /*%%%*/
        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::ID &node)
    {
        // im in
        if(DEBUG) /*%%%*/ std::cout << "entered id" << endl; /*%%%*/
        node.type_def = node.value;
    }

    void PrintVisitor::checkNode(ast::Exp &node)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.type_def);
        if (entry != nullEntry && entry.isFuck())
        {
            // in the symtab and is a fuck
            errorDefAsFunc(node.line, node.type_def);
        }
        else if (entry != nullEntry)
        {
            // in the symtab and is a veriant
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
            else
            {
                errorUndef(node.line, node.type_def);
            }
        }
    }

    void PrintVisitor::visit(ast::BinOp &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered binop" << endl; /*%%%*/

        // go inside recursively and return the values
        node.left->accept(*this);
        node.right->accept(*this);

        checkNode(*(node.left));
        checkNode(*(node.right));
        node.type_def = ((node.left->type_def.compare(toString(ast::BuiltInType::INT)) == 0) ||
                         (node.right->type_def.compare(toString(ast::BuiltInType::INT)) == 0)) ?
                         toString(ast::BuiltInType::INT) : toString(ast::BuiltInType::BYTE);
    }

    void PrintVisitor::visit(ast::RelOp &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered relop" << endl; /*%%%*/

        // go inside recursively and return the values
        node.left->accept(*this);
        node.right->accept(*this);

        checkNode(*(node.left));
        checkNode(*(node.right));
        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::Type &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered typush" << endl; /*%%%*/
        node.type_def = toString(node.getType());
    }

    void PrintVisitor::visit(ast::Cast &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered cast" << endl; /*%%%*/
        node.target_type->accept(*this);        // the explicit cast type
        node.exp->accept(*this);                // the current value

        if(node.target_type->getType() != ast::BuiltInType::INT &&
           node.target_type->getType() != ast::BuiltInType::BYTE) {
            errorMismatch(node.line);
        }
        checkNode(*(node.exp));
        node.type_def = node.target_type->type_def;
    }

    void PrintVisitor::assureBoolean(ast::Exp &node1, ast::Exp &node2, int nodeLine)
    {
        if (node1.type_def.compare(toString(ast::BuiltInType::BOOL)) != 0 ||
            node2.type_def.compare(toString(ast::BuiltInType::BOOL)) != 0)
        {
            errorMismatch(nodeLine);
        }
    }

    void PrintVisitor::visit(ast::Not &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered not" << endl; /*%%%*/
        node.exp->accept(*this);
        
        assureBoolean(*(node.exp),*(node.exp), node.line);

        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::And &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered and" << endl; /*%%%*/
        node.left->accept(*this);
        node.right->accept(*this);
        
        assureBoolean(*(node.left),*(node.right), node.line);

        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::Or &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered or" << endl; /*%%%*/
        node.left->accept(*this);
        node.right->accept(*this);
        
        assureBoolean(*(node.left),*(node.right), node.line);

        node.type_def = toString(ast::BuiltInType::BOOL);
    }

    void PrintVisitor::visit(ast::ExpList &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered explist" << endl; /*%%%*/

        for (auto it = node.exps.rbegin(); it != node.exps.rend(); ++it)
        {
            (*it)->accept(*this);
        }
    }

    void PrintVisitor::visit(ast::Call &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered call" << endl; /*%%%*/
        
        node.func_id->accept(*this);
        node.args->accept(*this);

        std::vector<std::string> givenArgs, expectedArgs;
        std::vector<ast::BuiltInType> formals;
        symbol_table::SymTableEntry entry =
            this->stack.isInsideSymolTable_byFallOutBoy(node.func_id->type_def);
        if ((entry == nullEntry))
        {
            // not in symtab
            errorUndefFunc(node.line, node.func_id->type_def);
        }
        else if (!entry.isFuck())
        {
            // it is but it is a var
            errorDefAsVar(node.line, node.func_id->type_def);
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
        for (auto it = sentFormals.begin(); it != sentFormals.end(); ++it)
        {
            //we have to search for the exp type bc maybe its a variable :(
            symbol_table::SymTableEntry entryExp = this->stack.isInsideSymolTable_byFallOutBoy((*it)->type_def);
            if (entryExp != nullEntry) {
                (*it)->type_def = toString(entryExp.getType());
            }
            givenArgs.push_back((*it)->type_def);
        }

        if (givenArgs != expectedArgs)
        {
            errorPrototypeMismatch(node.line, node.func_id->type_def, expectedArgs);
        }

        //set the functions return type as it was set in the entry
        node.type_def = toString(entry.getRetType());
    }

    void PrintVisitor::visit(ast::Statements &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered statements" << endl; /*%%%*/
        if (node.isInBraces)
        {
            this->printer.beginScope();
            this->stack.newScope();
            if (!node.getFormals().empty())
            {
                this->stack.addFormals(node.getFormals());
                std::vector<symbol_table::SymTableEntry> iter = this->stack.Head().GetDaBaby();
                for (auto it = iter.begin(); it != iter.end(); it++)
                {
                    this->printer.emitVar((*it).getNem(), (*it).getType(), (*it).getOff());
                }
            }
        }
        for (auto it = node.statements.begin(); it != node.statements.end(); ++it)
        {
            (*it)->accept(*this);
        }
        if (node.isInBraces)
        {
            this->printer.endScope();
            this->stack.popScope();
        }
    }

    void PrintVisitor::visit(ast::Break &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered break" << endl; /*%%%*/
        if (inLoop <= 0) { errorUnexpectedBreak(node.line); }
    }

    void PrintVisitor::visit(ast::Continue &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered continue" << endl; /*%%%*/
        if (inLoop <= 0) { errorUnexpectedContinue(node.line); }
    }

    void PrintVisitor::visit(ast::Return &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered return" << endl; /*%%%*/

        if (node.exp)
        {
            node.exp->accept(*this);
            cout << node.exp->type_def << endl;
            node.type_def = node.exp->type_def;
        }
        else {
            node.type_def = toString(ast::BuiltInType::VOID);
        }
    }

    bool PrintVisitor::isBoolean(ast::Exp &condition)
    {
        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(condition.type_def);
        if (entry != nullEntry && ((entry.isFuck() && (entry.getRetType() != ast::BuiltInType::BOOL)) ||
                                  (!entry.isFuck() && (entry.getType()    != ast::BuiltInType::BOOL)))) {
            cout << "huh???? 1" << endl;
            return false;
        }
        else if(entry != nullEntry) {
            condition.type_def = toString(ast::BuiltInType::BOOL);
        }

        if (condition.type_def.compare(toString(ast::BuiltInType::BOOL)) == 0) {
            return true;
        }
        return false;
    }

    void PrintVisitor::visit(ast::If &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered if" << endl; /*%%%*/
        
        node.condition->accept(*this);

        if(!isBoolean(*(node.condition))) {
            errorMismatch(node.line);
        }

        this->printer.beginScope();
        node.then->accept(*this);
        this->printer.endScope();

        if (node.otherwise)
        {
            this->printer.beginScope();
            node.otherwise->accept(*this);
            this->printer.endScope();
        }
    }

    void PrintVisitor::visit(ast::While &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered while" << endl; /*%%%*/
        
        node.condition->accept(*this);
        if(!isBoolean(*(node.condition))) {
            errorMismatch(node.line);
        }

        this->inLoop++;
        this->printer.beginScope();
        node.body->accept(*this);
        this->printer.endScope();
        this->inLoop--;
    }

    void PrintVisitor::assureAssignCorrect(std::shared_ptr<ast::Exp> exp, ast::BuiltInType nodeType,int nodeLine)
    {
        switch (nodeType)
        {
        case ast::BuiltInType::INT:
            // secong can by int or byte
            if ((exp->type_def.compare(toString(ast::BuiltInType::INT)) != 0 &&
                    exp->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0))
            {
                errorMismatch(nodeLine);
            }
            break;
        case ast::BuiltInType::BYTE:
            if (exp->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0)
            {
                errorMismatch(nodeLine);
            }
            // is the value too big to fit the byte?
            if (std::dynamic_pointer_cast<ast::NumB>(exp)->value > MAX_BYTE)
            {
                errorByteTooLarge(nodeLine, std::dynamic_pointer_cast<ast::NumB>(exp)->value);
            }
            break;
        default:
            // mayhaps a string
            errorMismatch(nodeLine);
            break;
        }
    }

    void PrintVisitor::visit(ast::VarDecl &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered vardecl" << endl; /*%%%*/

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
            errorDef(node.line, node.id->getVal());
        }

        if (initexpExists) {
            assureAssignCorrect(node.init_exp, node.type->getType(), node.line);
        }
        this->stack.newSymbol(node.id->getVal(), node.type->getType());
        // the latest offset is the offset added to the symboltable
        this->printer.emitVar(node.id->getVal(), node.type->getType(), this->stack.getLatestOffset());
    }

    void PrintVisitor::visit(ast::Assign &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered assign" << endl; /*%%%*/

        node.id->accept(*this);
        node.exp->accept(*this);

        symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy(node.id->getVal());
        if (entry == nullEntry)
        {
            errorUndef(node.line, node.id->getVal());
        }
        else if (entry.isFuck())
        {
            // this is fuck
            errorDefAsFunc(node.line, node.id->getVal());
        }

        assureAssignCorrect(node.exp, entry.getType(), node.line);
    }

    void PrintVisitor::visit(ast::Formal &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered formal" << endl; /*%%%*/

        node.id->accept(*this);
        node.type->accept(*this);
    }

    void PrintVisitor::visit(ast::Formals &node)
    {
        if(DEBUG)/*%%%*/ cout << "entered formals" << endl; /*%%%*/

        for (auto it = node.formals.rbegin(); it != node.formals.rend(); ++it)
        {
            (*it)->accept(*this);
        }
    }

    void PrintVisitor::visit(ast::FuncDecl &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered funcdecl" << endl; /*%%%*/

        std::vector<std::shared_ptr<ast::Formal>> formalsToBeAdded = node.getFormals();
        //check if the formal's names suits a function name
        for (auto it = formalsToBeAdded.begin(); it != formalsToBeAdded.end(); ++it)
        {
            symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy((*it)->id->getVal());
            if (entry != nullEntry) {
                errorDef(node.line, (*it)->id->getVal());
            }
        }
        this->stack.addFormals(formalsToBeAdded);

        node.id->accept(*this);
        node.return_type->accept(*this);
        node.formals->accept(*this);
        node.body->accept(*this);
        
        //check if there are returns - need to make sure the types are correct
        std::vector<std::shared_ptr<ast::Statement>> statements = node.body->statements;
        for (auto it = statements.begin(); it != statements.end(); ++it)
        {
            if ((*it)->isReturn) {
                if (node.return_type->type_def.compare(toString(ast::BuiltInType::INT)) == 0) {
                    //check if int - then check if int or byte
                    if (((*it)->type_def.compare(toString(ast::BuiltInType::INT)) !=0) &&
                        ((*it)->type_def.compare(toString(ast::BuiltInType::BYTE)) != 0)) {
                            errorMismatch(node.line);
                        }
                    }
                    //else - all the other types need to return the same type
                else if ((*it)->type_def.compare(node.return_type->type_def) != 0) {
                    errorMismatch(node.line);
                }
            }
        }
    }

    void PrintVisitor::visit(ast::Funcs &node)
    {
        if(DEBUG) /*%%%*/ cout << "entered funcs" << endl; /*%%%*/

        bool isThereMain = false;
        for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it)
        { // emit functions decls
            symbol_table::SymTableEntry entry = this->stack.isInsideSymolTable_byFallOutBoy((*it)->getID());
            if (entry != nullEntry)
            {
                errorDef((*it)->line, (*it)->getID());
            }
            //check if the funcdecl is main
            if (!isThereMain) {
                isThereMain = this->stack.checkIfMain((*it)->getID(), (*it)->getType(), (*it)->formalTypes());
            }
            this->printer.emitFunc((*it)->getID(), (*it)->getType(), (*it)->formalTypes());
            this->stack. newSymbol((*it)->getID(), ast::BuiltInType::FUCK, (*it)->getType(), (*it)->formalTypes());
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
        std::cout << this->printer; // prints the sexy beast that is called the program
    }
}
