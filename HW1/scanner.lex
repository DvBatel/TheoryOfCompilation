%{

/* Declarations section */
#include <iostream>
#include "tokens.hpp"
#include "output.hpp"
%}

%option yylineno
%option noyywrap
string          \"([^\\\"\n\r]|\\.)*
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
relop           (==|[!<>]=|[<>])
binop           ([\+\-\*\/])
comment         (\/\/.*)
id              ([a-zA-Z][a-zA-Z0-9]*)
num             ((0|[1-9][0-9]*))
whitespace      ([\n\t\r ])


%%
{string}(\")    return STRING;
{string}        output::errorUnclosedString();
{void}          return VOID;
{int}           return INT;
{byte}          return BYTE;
{bool}          return BOOL;
{and}           return AND;
{or}            return OR;
{not}           return NOT;
{true}          return TRUE;
{false}         return FALSE;
{return}        return RETURN;
{if}            return IF;
{else}          return ELSE;
{while}         return WHILE;
{break}         return BREAK;
{continue}      return CONTINUE;
{sc}            return SC;
{comma}         return COMMA;
{lparen}        return LPAREN;
{rparen}        return RPAREN;
{lbrace}        return LBRACE;
{rbrace}        return RBRACE;
{assign}        return ASSIGN;
{relop}         return RELOP;
{binop}         return BINOP;
{comment}       return COMMENT;
{id}            return ID;
{num}b          return NUM_B;
{num}           return NUM;
{whitespace}    ;
.		        output::errorUnknownChar(yytext[0]);
%%