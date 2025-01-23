%{

/* Declarations section */
#include <iostream>
#include "output.hpp"
#include "parser.tab.h"

extern YYSTYPE yylval;
using namespace ast;
%}

%option yylineno
%option noyywrap

string          \"([^\\\"\n\r]|\\[rnt\"\\])+\"
void            void
int             int
byte            byte
bool            bool
and             and
or              or
not             not
true            true
false           false
return          return
if              if
else            else
while           while
break           break
continue        continue
sc              ;
comma           ,
lparen          \(
rparen          \)
lbrace          \{
rbrace          \}
assign          =
eq              ==
ne              !=
lt              <
gt              >
le              <=
ge              >=
add             \+
sub             \-
mul             \*
div             \/
comment         (\/\/[^\r\n]*[\r|\n|\r\n]?)
id              ([a-zA-Z][a-zA-Z0-9]*)
num             ((0|[1-9][0-9]*))
whitespace      ([\n\t\r ])


%%
{string}        { yylval = std::make_shared<ast::String>(yytext);          return yytokentype::STRING;   }
{void}          { yylval = std::make_shared<ast::Type>(BuiltInType::VOID); return yytokentype::VOID;     }
{int}           { yylval = std::make_shared<ast::Type>(BuiltInType::INT);  return yytokentype::INT;      }
{byte}          { yylval = std::make_shared<ast::Type>(BuiltInType::BYTE); return yytokentype::BYTE;     }
{bool}          { yylval = std::make_shared<ast::Type>(BuiltInType::BOOL); return yytokentype::BOOL;     }
{and}           {                                                          return yytokentype::AND;      }
{or}            {                                                          return yytokentype::OR;       }
{not}           {                                                          return yytokentype::NOT;      }
{true}          { yylval = std::make_shared<ast::Bool>(true);              return yytokentype::TRUE;     }
{false}         { yylval = std::make_shared<ast::Bool>(false);             return yytokentype::FALSE;    }
{return}        {                                                          return yytokentype::RETURN;   }
{if}            {                                                          return yytokentype::IF;       }
{else}          {                                                          return yytokentype::ELSE;     }
{while}         {                                                          return yytokentype::WHILE;    }
{break}         {                                                          return yytokentype::BREAK;    }
{continue}      {                                                          return yytokentype::CONTINUE; }
{sc}            {                                                          return yytokentype::SC;       }
{comma}         {                                                          return yytokentype::COMMA;    }
{lparen}        {                                                          return yytokentype::LPAREN;   }
{rparen}        {                                                          return yytokentype::RPAREN;   }
{lbrace}        {                                                          return yytokentype::LBRACE;   }
{rbrace}        {                                                          return yytokentype::RBRACE;   }
{assign}        {                                                          return yytokentype::ASSIGN;   }
{eq}            {                                                          return yytokentype::EQ;       }
{ne}            {                                                          return yytokentype::NE;       }
{lt}            {                                                          return yytokentype::LT;       }
{gt}            {                                                          return yytokentype::GT;       }
{le}            {                                                          return yytokentype::LE;       }
{ge}            {                                                          return yytokentype::GE;       }
{add}           {                                                          return yytokentype::ADD;      }
{sub}           {                                                          return yytokentype::SUB;      }
{mul}           {                                                          return yytokentype::MUL;      }
{div}           {                                                          return yytokentype::DIV;      }
{id}            { yylval = std::make_shared<ast::ID>(yytext);              return yytokentype::ID;       }
{num}b          { yylval = std::make_shared<ast::NumB>(yytext);            return yytokentype::NUM_B;    }
{num}           { yylval = std::make_shared<ast::Num>(yytext);             return yytokentype::NUM;      }
{comment}       ;
{whitespace}    ;
.		        { output::errorLex(yylineno); }
%%