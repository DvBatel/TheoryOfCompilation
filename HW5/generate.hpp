#ifndef GENERATE_HPP
#define GENERATE_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iostream> //for testing
#include "nodes.hpp"
#include "stack.hpp"
#include "symbolTable.hpp"


namespace generate
{
    /* CodeBuffer class
     * This class is used to store the generated code.
     * It provides a simple interface to emit code and manage labels and variables.
     */
    class CodeBuffer
    {
    private:
        std::stringstream globalsBuffer;
        std::stringstream buffer;
        int labelCount;
        int varCount;
        int stringCount;

        friend std::ostream &operator<<(std::ostream &os, const CodeBuffer &buffer);

    public:
        CodeBuffer();

        // Returns a string that represents a label not used before
        // Usage examples:
        //      emitLabel(freshLabel());
        //      buffer << "br label " << freshLabel() << std::endl;
        std::string freshLabel();

        // Returns a string that represents a variable not used before
        // Usage examples:
        //      std::string var = freshVar();
        //      buffer << var << " = icmp eq i32 0, 0" << std::endl;
        std::string freshVar();

        // Emits a label into the buffer
        void emitLabel(const std::string &label);

        // Emits a constant string into the globals section of the code.
        // Returns the name of the constant. For the string of the length n (not including null character), the type is [n+1 x i8]
        // Usage examples:
        //      std::string str = emitString("Hello, World!");
        //      buffer << "call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* " << str << ", i32 0, i32 0))" << std::endl;
        std::string emitString(const std::string &str);

        // Emits a string into the buffer
        void emit(const std::string &str);

        void declare_externs();

        void define_prints();

        // Template overload for general types
        template <typename T>
        CodeBuffer &operator<<(const T &value)
        {
            buffer << value;
            return *this;
        }

        // Overload for manipulators (like std::endl)
        CodeBuffer &operator<<(std::ostream &(*manip)(std::ostream &));

    };

    std::ostream &operator<<(std::ostream &os, const CodeBuffer &buffer);

    /* GenCode class
     * This class is used to generate the llvm code using the code buffer.
     */
    class GenCode
    {
    private:
        generate::CodeBuffer buffer;
        stack::Stack *stack;

        std::string loop_head = "";
        std::string loop_end = "";

    public:
        GenCode() : buffer() { }

        ~GenCode() {}

        void setStack(stack::Stack *stack) { this->stack = stack; }

        void setHead(std::string head) { this->loop_head = head; }

        void setEnd(std::string end) { this->loop_end = end; }

        std::string getHead() { return this->loop_head; }

        std::string getEnd() { return this->loop_end; }

        std::string allocateFunctionStack();

        std::string generateLoadVar(string rbp, int offset  , string type);

        void generateStoreVar(string rbp, int offset, string reg, string type);
        
        void genZeroDiv();

        void genNum(ast::Num &node);

        void genNumB(ast::NumB &node);

        void genString(ast::String &node);

        void genBool(ast::Bool &node);

        void genID(ast::ID &node);

        void genBinOp(ast::BinOp &node);

        void genRelOp(ast::RelOp &node);

        void genNot(ast::Not &node);

        void genAndLeft(ast::And &node);

        void genAndRight(ast::And &node);

        void genOrLeft(ast::Or &node);

        void genOrRight(ast::Or &node);

        void genType(ast::Type &node);

        void genCast(ast::Cast &node);

        void genExpList(ast::ExpList &node);

        void genCall(ast::Call &node);

        void genStatements(ast::Statements &node);

        void genBreak(ast::Break &node);

        void genContinue(ast::Continue &node);

        void genReturn(ast::Return &node);

        void genIfCondition(ast::If &node);

        void genIfThen(ast::If &node);

        void genIfOtherwise(ast::If &node);

        void genWhileCondition(ast::While &node);

        void genWhileBody(ast::While &node);

        void genWhileEnd(ast::While &node);

        void genVarDecl(ast::VarDecl &node);

        void genAssign(ast::Assign &node);

        void genFormal(ast::Formal &node);

        void genFormals(ast::Formals &node);

        void genFuncDeclEntery(ast::FuncDecl &node);

        void genFuncDeclClosery(ast::FuncDecl &node);

        void genFuncs(ast::Funcs &node);

        void printCode();
    };
}

#endif // GENERATE_HPP
