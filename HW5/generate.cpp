#include "generate.hpp"

namespace generate
{
    /* CodeBuffer class */

    CodeBuffer::CodeBuffer() : labelCount(0), varCount(0), stringCount(0) {}

    std::string CodeBuffer::freshLabel()
    {
        return "%label_" + std::to_string(labelCount++);
    }

    std::string CodeBuffer::freshVar()
    {
        return "%t" + std::to_string(varCount++);
    }

    std::string CodeBuffer::emitString(const std::string &str)
    {
        std::string var = "@.str" + std::to_string(stringCount++);
        globalsBuffer << var << " = constant [" << str.length() + 1 << " x i8] c\"" << str << "\\00\"";
        return var;
    }

    void CodeBuffer::emit(const std::string &str)
    {
        buffer << str << std::endl;
    }

    void CodeBuffer::emitLabel(const std::string &label)
    {
        buffer << label.substr(1) << ":" << std::endl;
    }

    CodeBuffer &CodeBuffer::operator<<(std::ostream &(*manip)(std::ostream &))
    {
        buffer << manip;
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const CodeBuffer &buffer)
    {
        os << buffer.globalsBuffer.str() << std::endl
           << buffer.buffer.str();
        return os;
    }

    /* GenCode class */

    void GenCode::genZeroDiv()
    {
        this->buffer.emit("@.DIV_BY_ZERO_ERROR = internal constant [23 x i8] c\"Error division by zero\\00\"");

        // Division by zero check function
        buffer.emit("define void @check_division(i32) {");
        buffer.emit("%valid = icmp eq i32 %0, 0");
        buffer.emit("br i1 %valid, label %ILLEGAL, label %LEGAL");
        buffer.emit("ILLEGAL:");
        buffer.emit("call void @print(i8* getelementptr([23 x i8], [23 x i8]* @.DIV_BY_ZERO_ERROR, i32 0, i32 0))");
        buffer.emit("call void @exit(i32 0)");
        buffer.emit("ret void");
        buffer.emit("LEGAL:");
        buffer.emit("ret void");
        buffer.emit("}");
    }
    void GenCode::genNum(ast::Num &node)
    {
        // holds the numeric value what are ya gonna do about it
        node.reg = std::to_string(node.value);
    }

    void GenCode::genNumB(ast::NumB &node)
    {
        // holds the numeric value what are ya gonna do about it
        node.reg = std::to_string(node.value);
    }

    void GenCode::genString(ast::String &node)
    {
        // returns an actual register
        node.reg = this->buffer.emitString(node.value);
    }

    void GenCode::genBool(ast::Bool &node)
    {
        node.reg = node.value ? "1" : "0";
    }

    /* check if there's a register related to a var.
     * need to make sure in other functions we don't put him
     * on the left side of the command (big no no in llvm)*/
    void GenCode::genID(ast::ID &node)
    {
        symbol_table::SymTableEntry entry = this->stack->isInsideSymolTable_byFallOutBoy(node.value);
        if (entry != nullEntry)
        {
            return;
        }
        else if (!entry.isFuck())
        {
            node.reg = entry.getEmittedVarName();
        }
        // else - is func - no var needed (we use the function name)
    }

    //%binop_result = op i32 %left_value, %right_value
    void GenCode::genBinOp(ast::BinOp &node)
    {
        std::string op;
        switch (node.op)
        {
        case ast::BinOpType::ADD:
            op = "add";
            break;
        case ast::BinOpType::SUB:
            op = "sub";
            break;
        case ast::BinOpType::MUL:
            op = "mul";
            break;
        case ast::BinOpType::DIV:
            if (node.type_def.compare("int") == 0)
            {
                op = "sdiv";
            }
            else
            {
                op = "udiv";
            }
            break;
        }

        std::string new_reg = this->buffer.freshVar();
        std::string llvm_line = new_reg + " = " + op + " i32 " + node.left->reg + ", " + node.right->reg;
        if (op == "/")
        {
            buffer.emit("call void @check_division(i32 " + node.right->reg + ")");
            this->buffer.emit(llvm_line);
        }
        else
        {
            this->buffer.emit(llvm_line);
            if (node.type_def.compare("byte") == 0)
            {
                string old_reg = node.reg;
                node.reg = this->buffer.freshVar();
                this->buffer.emit(node.reg + " = and i32 255, " + old_reg);
            }
            else
            {
                node.reg = new_reg;
            }
        }
    }

    //%relop_result = icmp op i32 %left_value, %right_value
    void GenCode::genRelOp(ast::RelOp &node)
    {
        std::string op;
        switch (node.op)
        {
        case ast::RelOpType::EQ:
            op = "eq";
            break;
        case ast::RelOpType::NE:
            op = "ne";
            break;
        case ast::RelOpType::LT:
            op = "slt";
            break;
        case ast::RelOpType::LE:
            op = "sle";
            break;
        case ast::RelOpType::GT:
            op = "sgt";
            break;
        case ast::RelOpType::GE:
            op = "sge";
            break;
        }
        std::string new_reg = this->buffer.freshVar();
        std::string llvm_line = new_reg + " = icmp " + op + " i32 " + node.left->reg + ", " + node.right->reg;
        // emitting
        this->buffer.emit(llvm_line);
        // remembering who holds the information calculated
        node.reg = new_reg;
    }

    //%not_result = xor i32 %value, true
    void GenCode::genNot(ast::Not &node)
    {
        std::string new_reg = this->buffer.freshVar();
        std::string llvm_line = new_reg + " = xor i32 " + node.exp->reg + ", true";
        this->buffer.emit(llvm_line);
        node.reg = new_reg;
    }

    //%and_result = and i32 %left_var, %right_var
    void GenCode::genAnd(ast::And &node)
    {
        std::string new_reg = this->buffer.freshVar();
        std::string llvm_line = new_reg + " = and i32 " + node.left->reg + ", " + node.right->reg;
        this->buffer.emit(llvm_line);
        node.reg = new_reg;
    }

    //%or_result = and i32 %left_var, %right_var
    void GenCode::genOr(ast::Or &node)
    {
        std::string new_reg = this->buffer.freshVar();
        std::string llvm_line = new_reg + " = or i32 " + node.left->reg + ", " + node.right->reg;
        this->buffer.emit(llvm_line);
        node.reg = new_reg;
    }

    void GenCode::genType(ast::Type &node)
    {
    }

    void GenCode::genCast(ast::Cast &node)
    {
    }

    void GenCode::genExpList(ast::ExpList &node)
    {
    }

    void GenCode::genCall(ast::Call &node)
    {
        // retrieve explist vals in llvm
        auto explist = node.args->getExpList();
        std::string emitted_args;
        for (auto it = explist.begin(); it != explist.end(); ++it)
        {
            if ((*it)->type_def.compare("string") == 0)
            {
                // need to generate [len + 1 x i8]
                int len = std::dynamic_pointer_cast<ast::String>(*it)->value.length() + 1;
                emitted_args.append(std::to_string(len).append(" x i8* ").append((*it)->reg));
            }
            else if ((*it)->type_def.compare("byte") == 0)
            {
                emitted_args.append("i8* ");
            }
            else
            {
                emitted_args.append("i32 ");
            }
            emitted_args.append((*it)->reg);

            if (it + 1 != explist.end())
                emitted_args.append(", "); // we not finished
        }

        if (node.func_id->type_def.compare("void") == 0)
            this->buffer.emit("call void @" + node.func_id->value + "(" + emitted_args + ")");
        else
        {
            std::string new_reg = this->buffer.freshVar();
            node.func_id->reg = new_reg;
            string emitted_return_type = (node.func_id->type_def.compare("byte") == 0) ? "i8*" : "i32"; // else: int or bool
            buffer.emit(node.func_id->reg + " = call " + emitted_return_type + " @" + node.func_id->value + "(" + emitted_args + ")");
        }
    }

    void GenCode::genStatements(ast::Statements &node)
    {
    }

    void GenCode::genBreak(ast::Break &node)
    {
    }

    void GenCode::genContinue(ast::Continue &node)
    {
    }

    void GenCode::genReturn(ast::Return &node)
    {
    }

    /*
     *; Conditional branch
     *br i1 %cond, label %then, label %else
     *
     *then:                      ; Label for "then" block
     * ; Instructions for the "then" block
     * br label %end
     *
     *else:                      ; Label for "else" block
     * ; Instructions for the "else" block
     * br label %end
     *
     *end:                       ; Label for "end" block
     * ; Instructions after the "if" statement
     */
    void GenCode::genIfCondition(ast::If &node)
    {
        std::string true_lab, false_lab, end_lab;
        // undertale mentioned!??!?!?!?!?!??!?!
        true_lab = this->buffer.freshLabel();
        false_lab = this->buffer.freshLabel();
        if (node.otherwise)
        {
            end_lab = this->buffer.freshLabel();
        }
        // we need to make sure this emit something good so llvm wont crash down
        this->buffer.emit("br i1 " + node.condition->reg + ", label " + true_lab + ", label " + false_lab + "\n");
        this->buffer.emitLabel(true_lab);
        node.true_lab = true_lab;
        node.false_lab = false_lab;
        node.end_lab = end_lab;
    }

    void GenCode::genIfThen(ast::If &node)
    {
        if (node.otherwise)
        {
            this->buffer.emit("br label " + node.end_lab);
        }
        this->buffer.emitLabel(node.false_lab);
    }

    void GenCode::genIfOtherwise(ast::If &node)
    {
        if (node.otherwise)
        {
            this->buffer.emit("br label " + node.end_lab + "\n");
        }
        this->buffer.emitLabel(node.end_lab);
        this->buffer.emit("\n");
    }

    /*
     *br label %loop_header          ; Initial jump to the loop header
     *
     *loop_header:                   ; Start of the loop
     *%cond = icmp ...               ; Evaluate the loop condition (result is i1)
     * br i1 %cond, label %loop_body, label %exit_block
     *
     *loop_body:                     ; Body of the loop
     *  ; Instructions for the loop body
     * br label %loop_header         ; Jump back to the loop header
     *
     *exit_block:                    ; Exit block
     * ; Instructions after the loop
     */
    void GenCode::genWhileCondition(ast::While &node)
    {
        std::string loop_head, loop_body, end_lab;
        // undertale mentioned!??!?!?!?!?!??!?!
        loop_head = this->buffer.freshLabel();
        loop_body = this->buffer.freshLabel();
        end_lab = this->buffer.freshLabel();
        // we need to make sure this emit something good so llvm wont crash down
        this->buffer.emit("br label " + loop_head);
        this->buffer.emitLabel(loop_head);
        node.loop_head = loop_head;
        node.loop_body = loop_body;
        node.end_lab = end_lab;
    }

    void GenCode::genWhileBody(ast::While &node)
    {
        this->buffer.emit("br i1 " + node.condition->reg + ", label " + node.loop_body + ", label " + node.end_lab + "\n");
        this->buffer.emitLabel(node.loop_body);
    }

    void GenCode::genWhileEnd(ast::While &node)
    {
        this->buffer.emit("\n");
        this->buffer.emitLabel(node.end_lab);
    }

    void GenCode::genVarDecl(ast::VarDecl &node)
    {
    }

    void GenCode::genAssign(ast::Assign &node)
    {
    }

    void GenCode::genFormal(ast::Formal &node)
    {
    }

    void GenCode::genFormals(ast::Formals &node)
    {
    }

    void GenCode::genFuncDecl(ast::FuncDecl &node)
    {
    }

    void GenCode::genFuncs(ast::Funcs &node)
    {
    }

    void GenCode::printCode()
    {
        std::cout << this->buffer;
    }
}