#include "generate.hpp"

// debugging print func
const int DEBUG = 1;
void debugprint(string a)
{
    if (DEBUG)
    {
        std::cout << ";" << " -- debug generate -- " << a << std::endl;
    }
}

namespace generate
{
    /* CodeBuffer class */

    CodeBuffer::CodeBuffer() : labelCount(0), varCount(0), stringCount(0) {}

    string CodeBuffer::freshLabel()
    {
        return "%label_" + std::to_string(labelCount++);
    }
    string CodeBuffer::freshVar()
    {
        return "%t" + std::to_string(varCount++);
    }

    string CodeBuffer::emitString(const string &str)
    {
        string var = "@.str" + std::to_string(stringCount++);
        globalsBuffer << var << " = constant [" << str.length() + 1 << " x i8] c\"" << str << "\\00\"";
        return var;
    }

    void CodeBuffer::emit(const string &str)
    {
        buffer << str << std::endl;
    }

    void CodeBuffer::emitLabel(const string &label)
    {
        buffer << label.substr(1) << ":" << std::endl;
    }

    CodeBuffer &CodeBuffer::operator<<(std::ostream &(*manip)(std::ostream &))
    {
        buffer << manip;
        return *this;
    }

    void CodeBuffer::declare_externs()
    {
        emit("@.intFormat = internal constant [4 x i8] c\"%d\\0A\\00\"");
        emit("@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c\"Error division by zero\\00\"\n");

        emit("declare i32 @scanf(i8*, ...)");
        emit("declare i32 @printf(i8*, ...)");
        emit("declare void @exit(i32)\n");

        emit("@.int_specifier_scan = constant [3 x i8] c\"%d\\00\"");
        emit("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
        emit("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"\n");
    }
    void CodeBuffer::define_prints()
    {
        emit("define i32 @readi(i32){");
        emit("%ret_val = alloca i32");
        emit("%spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0");
        emit("call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)");
        emit("%val = load i32, i32* %ret_val");
        emit("ret i32 %val");
        emit("}\n");

        emit("define void @printi(i32){");
        emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
        emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
        emit("ret void");
        emit("}\n");

        emit("define void @print(i8*){");
        emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
        emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
        emit("ret void");
        emit("}\n");
    }

    std::ostream &operator<<(std::ostream &os, const CodeBuffer &buffer)
    {
        os << buffer.globalsBuffer.str() << std::endl
           << buffer.buffer.str();
        return os;
    }

    /* GenCode class */

    string getLLVMType(ast::BuiltInType type)
    {
        switch (type)
        {
        case ast::BuiltInType::INT:
            return "i32";
            break;
        case ast::BuiltInType::BYTE:
            return "i8";
            break;
        case ast::BuiltInType::BOOL:
            return "i1";
            break;
        case ast::BuiltInType::VOID:
            return "void";
            break;
        default:
            // huh
            debugprint("how did i.... get here");
            debugprint("uhhhh can i get uhhhh " + to_string(type) + " please?");
            exit(46);
            break;
        }
    }

    string GenCode::generateLoadVar(string rbp, int offset, string type)
    {
        string reg = this->buffer.freshVar();
        string var_ptr = this->buffer.freshVar();
        buffer.emit(var_ptr + " = getelementptr i32, i32* " + rbp + ", i32 " + std::to_string(offset));
        buffer.emit(reg + " = load i32, i32* " + var_ptr);
        return reg;
    }

    void GenCode::generateStoreVar(string rbp, int offset, string reg, string type)
    {
        string var_ptr = this->buffer.freshVar();
        buffer.emit(var_ptr + " = getelementptr i32, i32* " + rbp + ", i32 " + std::to_string(offset));
        // if type is non stackable, we must zest like a lemon
        // %t5_ext = zext i8 %t5 to i32
        string lemon = reg;
        if (type.compare("i8") == 0)
        {
            lemon = buffer.freshVar();
            buffer.emit(lemon + " = zext i8 " + reg + " to i32");
        }
        else if (type.compare("i1") == 0)
        {
            lemon = buffer.freshVar();
            buffer.emit(lemon + " = zext i1 " + reg + " to i32");
        }

        buffer.emit("store i32 " + lemon + ", i32* " + var_ptr);
    }

    string GenCode::allocateFunctionStack()
    {
        string base_pointer = this->buffer.freshVar();
        buffer.emit(base_pointer + " = alloca i32, i32 75");
        return base_pointer;
    }

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
        buffer.emit("}\n");
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

    void GenCode::genID(ast::ID &node)
    {
        symbol_table::SymTableEntry entry = this->stack->isInsideSymolTable_byFallOutBoy(node.value);
        if (entry == nullEntry || entry.isFuck())
        {
            return; // "don't do shit" , Batel, 2025
        }
        else
        {
            // local var
            string type = getLLVMType(entry.getType());
            string var_ptr = this->buffer.freshVar();
            string rbp = this->stack->getBasePointer();

            this->buffer.emit(var_ptr + " = getelementptr i32, i32* " + rbp + ", i32 " + std::to_string(entry.getOff()));
            string reg = this->buffer.freshVar();
            this->buffer.emit(reg + " = load i32, i32* " + var_ptr);

            if (type.compare("i8") == 0) // byteify the element
            {
                string regreg = this->buffer.freshVar();
                this->buffer.emit(regreg + " = trunc i32 " + reg + " to i8");
                reg = regreg;
            }

            // Assign the computed value to the node's register
            node.reg = reg;
        }
    }

    //%binop_result = op i32 %left_value, %right_value
    void GenCode::genBinOp(ast::BinOp &node)
    {
        // 2 reg: reg_right reg_left
        string reg_right = node.right->reg;
        string reg_left = node.left->reg;
        string type = "i32";

        if (node.left->type_def.compare("int") == 0 && node.right->type_def.compare("byte") == 0)
        {
            // cast to right
            // %t5_ext = zext i8 %t5 to i32
            string reg = this->buffer.freshVar();
            this->buffer.emit(reg + " = zext i8 " + node.right->reg + " to i32");
            // now the correct reg is the new one
            reg_right = reg;
        }
        else if (node.left->type_def.compare("byte") == 0 && node.right->type_def.compare("int") == 0)
        {
            // cast to left
            string reg = this->buffer.freshVar();
            this->buffer.emit(reg + " = zext i8 " + node.left->reg + " to i32");
            // now the correct reg is the new one
            reg_left = reg;
        }
        else if (node.left->type_def.compare("byte") == 0 && node.right->type_def.compare("byte") == 0)
        {
            // then they both byte
            type = "i8";
        }
        // no more casting needed

        string op;
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
        // reg_right, reg_left are of the same type.
        string new_reg = this->buffer.freshVar();
        string llvm_line = new_reg + " = " + op + " " + type + " " + reg_left + ", " + reg_right;
        node.reg = new_reg;
        if (node.op == ast::BinOpType::DIV)
        {
            buffer.emit("call void @check_division(i32 " + reg_right + ")");
        }
        this->buffer.emit(llvm_line);
    }

    void GenCode::genIcmp(string new_reg, string op, string left_reg, string right_reg)
    {
        string llvm_line = new_reg + " = icmp " + op + " i32 " + left_reg + ", " + right_reg;
        this->buffer.emit(llvm_line);
    }

    void GenCode::genBoolIcmp(string new_reg, string reg)
    {
        string llvm_line = new_reg + " = icmp eq i1 " + reg + ", " + "1";
        this->buffer.emit(llvm_line);
    }

    //%relop_result = icmp op i32 %left_value, %right_value
    void GenCode::genRelOp(ast::RelOp &node)
    {
        string op;
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
        // if we are comparing different types:
        string reg_right = node.right->reg;
        string reg_left = node.left->reg;
        // %extended = zext i1 %cond to i32
        if (node.left->type_def.compare("byte") == 0)
        {
            reg_left = buffer.freshVar();
            buffer.emit(reg_left + " = zext i8 " + node.left->reg + " to i32 ");
        }
        if (node.right->type_def.compare("byte") == 0)
        {
            reg_right = buffer.freshVar();
            buffer.emit(reg_right + " = zext i8 " + node.right->reg + " to i32 ");
        }

        string new_reg = this->buffer.freshVar();
        string llvm_line = new_reg + " = icmp " + op + " i32 " + reg_left + ", " + reg_right;
        // emitting
        this->buffer.emit(llvm_line);
        // remembering who holds the information calculated
        node.reg = new_reg;
    }

    //%not_result = xor i32 %value, true
    void GenCode::genNot(ast::Not &node)
    {
        string new_reg = this->buffer.freshVar();
        string llvm_line = new_reg + " = xor i1 " + node.exp->reg + ", true";
        this->buffer.emit(llvm_line);
        node.reg = new_reg;
    }

    //%and_result = and i32 %left_var, %right_var
    void GenCode::genAndLeft(ast::And &node)
    {
        string check_right = this->buffer.freshLabel();
        string false_lab = this->buffer.freshLabel();
        string true_lab = this->buffer.freshLabel();
        string end_lab = this->buffer.freshLabel();

        node.false_lab = false_lab;
        node.true_lab = true_lab;
        node.end_lab = end_lab;

        // check left
        string new_reg = this->buffer.freshVar();
        genBoolIcmp(new_reg, node.left->reg);

        this->buffer.emit("br i1 " + new_reg + ", label " + check_right + ", label " + false_lab + "\n");
        this->buffer.emitLabel(check_right);
    }

    void GenCode::genAndRight(ast::And &node)
    {
        // check right
        string new_reg = this->buffer.freshVar();
        genBoolIcmp(new_reg, node.right->reg);
        this->buffer.emit("br i1 " + new_reg + ", label " + node.true_lab + ", label " + node.false_lab + "\n");

        this->buffer.emitLabel(node.false_lab);
        this->buffer.emit("br label " + node.end_lab + "\n");

        this->buffer.emitLabel(node.true_lab);
        this->buffer.emit("br label " + node.end_lab + "\n");

        this->buffer.emitLabel(node.end_lab);
        string phi_reg = this->buffer.freshVar();

        // %result = phi i32 [ true_reg, %if_true ], [ false_reg, %if_false ]
        string llvm_line = phi_reg + " = phi i1 [ true, " + node.true_lab + " ], [ false, " + node.false_lab + " ]\n";
        this->buffer.emit(llvm_line);
        node.reg = phi_reg;
    }

    //%or_result = and i32 %left_var, %right_var
    // this is shit, need to do some store shit i think
    void GenCode::genOrLeft(ast::Or &node)
    {
        string check_right = this->buffer.freshLabel();
        string false_lab = this->buffer.freshLabel();
        string true_lab = this->buffer.freshLabel();
        string end_lab = this->buffer.freshLabel();

        node.false_lab = false_lab;
        node.true_lab = true_lab;
        node.end_lab = end_lab;

        // check left
        string new_reg = this->buffer.freshVar();
        genBoolIcmp(new_reg, node.left->reg);

        this->buffer.emit("br i1 " + new_reg + ", label " + true_lab + ", label " + check_right + "\n");
        this->buffer.emitLabel(check_right);
    }

    void GenCode::genOrRight(ast::Or &node)
    {
        // check right
        string new_reg = this->buffer.freshVar();
        genBoolIcmp(new_reg, node.right->reg);
        this->buffer.emit("br i1 " + new_reg + ", label " + node.true_lab + ", label " + node.false_lab + "\n");

        this->buffer.emitLabel(node.false_lab);
        this->buffer.emit("br label " + node.end_lab + "\n");

        this->buffer.emitLabel(node.true_lab);
        this->buffer.emit("br label " + node.end_lab + "\n");

        this->buffer.emitLabel(node.end_lab);
        string phi_reg = this->buffer.freshVar();

        // %result = phi i32 [ true_reg, %if_true ], [ false_reg, %if_false ]
        string llvm_line = phi_reg + " = phi i1 [ true, " + node.true_lab + " ], [ false, " + node.false_lab + " ]\n";
        this->buffer.emit(llvm_line);
        node.reg = phi_reg;
    }

    // we recognize that this is a possible gen function but given it's a stupid ass gen function we've elected to ignore it
    void GenCode::genType(ast::Type &node) {}

    void GenCode::genCast(ast::Cast &node)
    {
        string src_type; // Get the source type
        if (node.exp->type_def.compare("int") == 0)
        {
            src_type = "i32";
        }
        else if (node.exp->type_def.compare("byte") == 0)
        {
            src_type = "i8";
        }
        else
        {
            // what
            debugprint("wheres the doOR");
            exit(0);
        }

        string dest_type = getLLVMType(node.target_type->getType()); // Get the destination type
        if (dest_type.compare(src_type) == 0)
        {
            node.reg = node.exp->reg; // save the result register in the node
            // all good no worries
            return;
        }

        string new_reg = this->buffer.freshVar(); // Get a new register name
        if (src_type == "i8" && dest_type == "i32")
        {
            // zeroextention from i8 to i32
            this->buffer.emit(new_reg + " = zext i8 " + node.exp->reg + " to i32");
        }
        else if (src_type == "i32" && dest_type == "i8")
        {
            // truncate from i32 to i8
            this->buffer.emit(new_reg + " = trunc i32 " + node.exp->reg + " to i8");
        }
        else
        {
            // huhh?
            debugprint("shit that wasn't supposed to happen - happened. plz give us 3 bonus points for creativity");
            exit(3);
        }
        node.reg = new_reg; // save the result register in the node
    }

    // we don't need ya
    void GenCode::genExpList(ast::ExpList &node) {}

    void GenCode::genCall(ast::Call &node)
    {
        // retrieve explist vals in llvm

        // we want to check if any bastard byte is trying to get in an int's place
        // it's okay he can do that but we kinda need to cast him for the passage to go through
        symbol_table::SymTableEntry entry = this->stack->isInsideSymolTable_byFallOutBoy(node.func_id->type_def);
        std::vector<ast::BuiltInType> formals_types_of_func = entry.getgetFormals();
        auto formal = formals_types_of_func.begin();

        auto explist = node.args->getExpList();
        string emitted_args;
        for (auto it = explist.begin(); it != explist.end(); ++it)
        {
            if ((*it)->type_def.compare("string") == 0)
            {
                // need to generate [len + 1 x i8]
                int len = std::dynamic_pointer_cast<ast::String>(*it)->value.length() + 1;
                emitted_args.append(string("[").append(std::to_string(len)).append(" x i8] ").append(", "));
                emitted_args.append(string("[").append(std::to_string(len)).append(" x i8]* ").append((*it)->reg));
            }
            else if ((*it)->type_def.compare("byte") == 0)
            {
                if ((*formal) == ast::BuiltInType::INT)
                {
                    string casted = buffer.freshVar();
                    buffer.emit(casted + " = zext i8 " + (*it)->reg + " to i32");
                    emitted_args.append("i32 ").append(casted);
                }
                else
                {
                    emitted_args.append("i8 ").append((*it)->reg);
                }
            }
            else if ((*it)->type_def.compare("bool") == 0)
            {
                if ((*formal) == ast::BuiltInType::INT)
                {
                    string casted = buffer.freshVar();
                    buffer.emit(casted + " = zext i1 " + (*it)->reg + " to i32");
                    emitted_args.append("i32 ").append(casted);
                }
                else if ((*formal) == ast::BuiltInType::BYTE)
                {
                    string casted = buffer.freshVar();
                    buffer.emit(casted + " = zext i1 " + (*it)->reg + " to i8");
                    emitted_args.append("i8 ").append(casted);
                }
                else
                {
                    emitted_args.append("i1 ").append((*it)->reg);
                }
            }
            else // int
            {
                emitted_args.append("i32 ").append((*it)->reg);
            }
            if (formal != formals_types_of_func.end()) {
                formal++;
            }
            if (it + 1 != explist.end())
                emitted_args.append(", "); // we not finished
        }

        if (entry.getRetType() == ast::BuiltInType::VOID)
        {
            if (node.func_id->value.compare("print") == 0)
            {
                this->buffer.emit("call void @print(i8* getelementptr (" + emitted_args + ", i32 0, i32 0))");
            }
            else
            {
                this->buffer.emit("call void @" + node.func_id->value + "(" + emitted_args + ")");
            }
        }
        else
        {
            string new_reg = this->buffer.freshVar();
            node.func_id->reg = new_reg;
            string emitted_return_type = getLLVMType(entry.getRetType());

            buffer.emit(node.func_id->reg + " = call " + emitted_return_type + " @" + node.func_id->value + "(" + emitted_args + ")");
            node.reg = node.func_id->reg;
        }
    }

    // fucking useless
    void GenCode::genStatements(ast::Statements &node) {}

    void GenCode::genBreak(ast::Break &node)
    {
        this->buffer.emit("br label " + getEnd());
    }

    void GenCode::genContinue(ast::Continue &node)
    {
        this->buffer.emit("br label " + getHead());
    }

    void GenCode::genReturn(ast::Return &node)
    {
        if (!node.exp)
            buffer.emit("ret void");
        else
        {
            string return_type;
            if (node.type_def.compare("string") == 0)
            {
                return_type = "i8*";
            }
            else if (node.type_def.compare("int") == 0)
            {
                return_type = "i32";
            }
            else if (node.type_def.compare("byte") == 0)
            {
                return_type = "i8";
            }
            else if (node.type_def.compare("bool") == 0)
            {
                return_type = "i1";
            }
            buffer.emit("ret " + return_type + " " + node.exp->reg);
        }
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
        string true_lab, false_lab, end_lab;
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
        else
        {
            this->buffer.emit("br label " + node.false_lab);
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
        string head_lab, body_lab, end_lab;
        head_lab = this->buffer.freshLabel();
        body_lab = this->buffer.freshLabel();
        end_lab = this->buffer.freshLabel();

        this->buffer.emit("br label " + head_lab);
        this->buffer.emitLabel(head_lab);
        node.loop_head = head_lab;
        node.loop_body = body_lab;
        node.end_lab = end_lab;

        setHead(head_lab);
        setEnd(end_lab);
    }

    void GenCode::genWhileBody(ast::While &node)
    {
        // need to make sure condition is always holding a register
        this->buffer.emit("br i1 " + node.condition->reg + ", label " + node.loop_body + ", label " + node.end_lab + "\n");
        this->buffer.emitLabel(node.loop_body);
    }

    void GenCode::genWhileEnd(ast::While &node)
    {
        this->buffer.emit("br label " + node.loop_head + "\n");
        this->buffer.emitLabel(node.end_lab);
    }

    void GenCode::genVarDecl(ast::VarDecl &node)
    {
        // assuming syntax is valid (in us we trust)
        symbol_table::SymTableEntry entry = this->stack->isInsideSymolTable_byFallOutBoy(node.id->value);
        string var_reg = this->buffer.freshVar();
        this->stack->setEmmited(node.id->getVal(), var_reg);
        // Allocate space for the variable on the stack
        string type_in_llvm = getLLVMType(node.type->getType());
        string init_val = "0"; // batel did not agree for this to be 3 :(()) shiran sad
        if (node.init_exp)
        {
            init_val = node.init_exp->reg;
            if (node.type->getType() == ast::BuiltInType::INT && node.init_exp->type_def.compare("byte") == 0)
            {
                string casted = buffer.freshVar();
                buffer.emit(casted + " = zext i8 " + init_val + " to i32");
                init_val = casted;
            }
        }
        this->buffer.emit(var_reg + " = add " + type_in_llvm + " 0, " + init_val);
        generateStoreVar(this->stack->getBasePointer(), entry.getOff(), var_reg, type_in_llvm);
    }

    void GenCode::genAssign(ast::Assign &node)
    {
        // feeling like genAssign? Try genOside
        symbol_table::SymTableEntry entry = this->stack->isInsideSymolTable_byFallOutBoy(node.id->getVal());
        string var_reg = entry.getEmittedName();

        this->stack->setEmmited(node.id->getVal(), var_reg);
        string type_in_llvm = getLLVMType(entry.getType());
        // Emit store instruction to update the variable
        generateStoreVar(this->stack->getBasePointer(), entry.getOff(), node.exp->reg, type_in_llvm);
    }

    // useless
    void GenCode::genFormal(ast::Formal &node) {}

    void GenCode::genFormals(ast::Formals &node)
    {
        /*
        a b c d e f g
        what the fuck is wrong with me
        i cant sleep
        i cant eat
        found one bug and lost my shit

        code generation is fun
        swear i did not kill no one
        */
    }

    void GenCode::genFuncDeclEntery(ast::FuncDecl &node)
    {
        // need ret either way. two rets never killed anyone
        // honestly if its such a big deal the optimizor bitch will handle it
        // fucking allocate the base stack
        string func_ret_type = getLLVMType(node.getType());

        string emitted_formals = "";

        // need to set formals registers
        auto formal = node.formals->getFormals();
        auto it_formal = formal.begin();
        int i = 0;
        
        for (auto it = formal.begin(); it != formal.end(); ++it)
        {
            string reg = string("%arg").append(to_string(i));
            debugprint(reg + "dfjnkdj " + (*it_formal)->id->getVal());
            emitted_formals.append(getLLVMType((*it)->type->getType())).append(" ").append(reg);

            i++;
            it_formal++;
            if (it + 1 != formal.end())
                emitted_formals.append(", "); // we not finished
        }

        this->buffer.emit("define " + func_ret_type + " @" + node.getID() + "(" + emitted_formals + "){");
        this->stack->setBasePointer(allocateFunctionStack());

        // save formals in stack
        i = 0;
        int offset = -1;
        for (auto it = formal.begin(); it != formal.end(); ++it) {
            generateStoreVar(this->stack->getBasePointer(), offset, string("%arg").append(to_string(i)), getLLVMType((*it)->type->getType()));
            i++;
            offset--;
        }
    }

    void GenCode::genFuncDeclClosery(ast::FuncDecl &node)
    {
        // in case the function is a void but the user have abstained from adding a 'return;' we will do it
        // this way the block will be well defined and we might actually pass this fucking assigment
        if (node.getType() == ast::BuiltInType::VOID)
            this->buffer.emit("ret void\n}");
        else
            this->buffer.emit("}");
        this->stack->popBasePointer();
    }

    // another useless bitch
    void GenCode::genFuncs(ast::Funcs &node)
    {
        genZeroDiv();
        this->buffer.declare_externs();
        this->buffer.define_prints();
    }

    void GenCode::printCode()
    {
        std::cout << this->buffer;
    }
}