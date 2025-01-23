%{
#include "nodes.hpp"
#include "output.hpp"
#include <stdio.h>
#include <iostream>

// bison declarations
extern int yylineno;
extern int yylex();

void yyerror(const char* message);

// root of the AST, set by the parser and used by other parts of the compiler
std::shared_ptr<ast::Node> program;

using namespace std;
using namespace ast;
using namespace output;

BinOpType binOpEnum(char op);
RelOpType relOpEnum(string op);

// TODO: Place any additional declarations here
// no.

%}

// TODO: Define tokens here

%token VOID INT BYTE BOOL CONST NUM_B NUM STRING
%token TRUE FALSE IF RETURN WHILE BREAK CONTINUE SC COMMA COMMENT ID
%token LPAREN RPAREN LBRACE RBRACE ASSIGN NOT ELSE
%token ADD SUB MUL DIV EQ NE LT GT LE GE


%nonassoc VOID
%nonassoc INT
%nonassoc BYTE
%nonassoc NUM_B
%nonassoc BOOL
%nonassoc CONST
%nonassoc TRUE
%nonassoc FALSE
%nonassoc IF
%nonassoc RETURN
%nonassoc WHILE
%nonassoc BREAK
%nonassoc CONTINUE
%nonassoc SC
%nonassoc COMMA
%nonassoc COMMENT
%nonassoc ID
%nonassoc NUM
%nonassoc STRING

// le hirarchy♥
%left OR
%left AND

%left EQ NE LT GT LE GE
%left ADD SUB
%left MUL DIV
%right ASSIGN
%right NOT
%left LPAREN RPAREN LBRACE RBRACE
%right ELSE

%%

// da rules (guidlines, really)

// we are we are PROGRAM 
// While reducing the start variable, set the root of the AST
Program: Funcs                                                      {  program = $1; }
;
Funcs: /*epsilon*/												    {  $$ = std::make_shared<ast::Funcs>(); }
	   | FuncDecl Funcs 											{  (std::dynamic_pointer_cast<ast::Funcs>($2))->push_front(std::dynamic_pointer_cast<ast::FuncDecl>($1));
																	   $$ = $2; }
;
FuncDecl: RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE {  std::dynamic_pointer_cast<ast::Statements>($7)->isInBraces = true;
                                                                       $$ = std::make_shared<ast::FuncDecl>(std::dynamic_pointer_cast<ast::ID>($2),
                                                                                                           std::dynamic_pointer_cast<ast::Type>($1),
                                                                                                           std::dynamic_pointer_cast<ast::Formals>($4),
                                                                                                           std::dynamic_pointer_cast<ast::Statements>($7)); }
;
RetType: Type 														{  $$ = $1; }
       | VOID 														{  $$ = $1; }
;
Formals:  /*epsilon*/												{  $$ = std::make_shared<ast::Formals>(); }
		 |FormalsList 												{  $$ = $1; }
;
FormalsList: FormalDecl COMMA FormalsList 							{  (std::dynamic_pointer_cast<ast::Formals>($3))->push_front(std::dynamic_pointer_cast<ast::Formal>($1));
                                                                       $$ = $3; }
            | FormalDecl 											{  $$ = std::make_shared<ast::Formals>(std::dynamic_pointer_cast<ast::Formal>($1)); }
;
FormalDecl: Type ID 												{  $$ = std::make_shared<ast::Formal>(std::dynamic_pointer_cast<ast::ID>($2),
                                                                                                         std::dynamic_pointer_cast<ast::Type>($1)); }
;
Statements: Statements Statement 									{  (std::dynamic_pointer_cast<ast::Statements>($1))->push_back(std::dynamic_pointer_cast<ast::Statement>($2));
                                                                       $$ = $1; }
            | Statement 											{  $$ = std::make_shared<ast::Statements>(std::dynamic_pointer_cast<ast::Statement>($1)); }

;
Statement: Type ID SC 												{  $$ = std::make_shared<ast::VarDecl>(std::dynamic_pointer_cast<ast::ID>($2),
                                                                                                          std::dynamic_pointer_cast<ast::Type>($1)); }
         | Type ID ASSIGN Exp SC 									{  $$ = std::make_shared<ast::VarDecl>(std::dynamic_pointer_cast<ast::ID>($2),
                                                                                                          std::dynamic_pointer_cast<ast::Type>($1),
                                                                                                          std::dynamic_pointer_cast<ast::Exp>($4)); }
         | LBRACE Statements RBRACE 								{  std::dynamic_pointer_cast<ast::Statements>($2)->isInBraces = true;
                                                                       $$ = $2; }
         | ID ASSIGN Exp SC 										{  $$ = std::make_shared<ast::Assign>(std::dynamic_pointer_cast<ast::ID>($1),
                                                                                                         std::dynamic_pointer_cast<ast::Exp>($3)); }
         | Call SC 													{  $$ = $1; }
         | RETURN SC 												{  $$ = std::make_shared<ast::Return>(nullptr);
                                                                       std::dynamic_pointer_cast<ast::Return>($$)->isReturn = true; }
         | RETURN Exp SC 											{  $$ = std::make_shared<ast::Return>(std::dynamic_pointer_cast<ast::Exp>($2));
                                                                       std::dynamic_pointer_cast<ast::Return>($$)->isReturn = true;  }
         | IF LPAREN Exp RPAREN Statement ELSE Statement 			{  $$ = std::make_shared<ast::If>(std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                     std::dynamic_pointer_cast<ast::Statement>($5),
                                                                                                     std::dynamic_pointer_cast<ast::Statement>($7)); }
         | IF LPAREN Exp RPAREN Statement 							{  $$ = std::make_shared<ast::If>(std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                     std::dynamic_pointer_cast<ast::Statement>($5)); }
         | WHILE LPAREN Exp RPAREN Statement 						{  $$ = std::make_shared<ast::While>(std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                        std::dynamic_pointer_cast<ast::Statement>($5)); }
         | BREAK SC 												{  $$ = std::make_shared<ast::Break>(); }
         | CONTINUE SC 												{  $$ = std::make_shared<ast::Continue>(); }
;
Call: ID LPAREN ExpList RPAREN										{  $$ = std::make_shared<ast::Call>(std::dynamic_pointer_cast<ast::ID>($1),
                                                                                                       std::dynamic_pointer_cast<ast::ExpList>($3)); }
    | ID LPAREN RPAREN 												{  $$ = std::make_shared<ast::Call>(std::dynamic_pointer_cast<ast::ID>($1)); }
;
ExpList: Exp COMMA ExpList 											{  (std::dynamic_pointer_cast<ast::ExpList>($3))->push_front(std::dynamic_pointer_cast<ast::Exp>($1));
                                                                       $$ = $3; }
       | Exp 														{  $$ = std::make_shared<ast::ExpList>(std::dynamic_pointer_cast<ast::Exp>($1)); }
;
Type: INT 															{  $$ = $1; }
    | BYTE 															{  $$ = $1; }
    | BOOL															{  $$ = $1; }
;
Exp: LPAREN Exp RPAREN 										        {  $$ = $2; }
   | Exp MUL Exp 													{  $$ = std::make_shared<ast::BinOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                        std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                        binOpEnum('*')); }
   | Exp DIV Exp 													{  $$ = std::make_shared<ast::BinOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                        std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                        binOpEnum('/')); }
   | Exp ADD Exp 													{  $$ = std::make_shared<ast::BinOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                        std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                        binOpEnum('+')); }
   | Exp SUB Exp 													{  $$ = std::make_shared<ast::BinOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                        std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                        binOpEnum('-')); }
   | LPAREN Type RPAREN Exp 										{  $$ = std::make_shared<ast::Cast>(std::dynamic_pointer_cast<ast::Exp>($4),
                                                                                                       std::dynamic_pointer_cast<ast::Type>($2)); }
   | Call 															{  $$ = $1; }
   | ID 															{  $$ = $1; }
   | NUM_B 															{  $$ = $1; }
   | NUM 															{  $$ = $1; }
   | STRING 														{  $$ = $1; }
   | TRUE 															{  $$ = $1; }
   | FALSE 															{  $$ = $1; }
   | NOT Exp 														{  auto node = std::dynamic_pointer_cast<ast::Exp>($2);
                                                                       $$ = std::make_shared<ast::Not>(std::dynamic_pointer_cast<ast::Exp>($2)); }
   | Exp AND Exp 													{  auto node1 = std::dynamic_pointer_cast<ast::Exp>($1);
                                                                       auto node2 = std::dynamic_pointer_cast<ast::Exp>($3);
                                                                       $$ = std::make_shared<ast::And>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3)); }
   | Exp OR Exp 													{  $$ = std::make_shared<ast::Or>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                      std::dynamic_pointer_cast<ast::Exp>($3)); }
   | Exp EQ Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string("=="))); }
   | Exp NE Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string("!="))); }
   | Exp LT Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string("<"))); }
   | Exp GT Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string(">"))); }
   | Exp LE Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string("<="))); }
   | Exp GE Exp 													{  $$ = std::make_shared<ast::RelOp>(std::dynamic_pointer_cast<ast::Exp>($1),
                                                                                                       std::dynamic_pointer_cast<ast::Exp>($3),
                                                                                                       relOpEnum(std::string(">="))); }
;


%%
// TODID: Place any additional code here


void yyerror(const char * message){
	output::errorSyn(yylineno);
	exit(0);
}

BinOpType binOpEnum(char op) {
    if (op == '*') return ast::BinOpType::MUL;
    if (op == '/') return ast::BinOpType::DIV;
    if (op == '+') return ast::BinOpType::ADD;
    else return ast::BinOpType::SUB;

}

RelOpType relOpEnum(string op) {
    if (!op.compare("==")) return RelOpType::EQ;
    if (!op.compare("!=")) return RelOpType::NE;
    if (!op.compare("<")) return RelOpType::LT;
    if (!op.compare(">")) return RelOpType::GT;
    if (!op.compare("<=")) return RelOpType::LE;
    else return ast::RelOpType::GE;

}

// bison more like byeeeeeee
