#include "output.hpp"
#include "nodes.hpp"
#include <iostream>

// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();
    // in memory ♥
    // Print the AST using the PrintVisitor
    output::PrintVisitor printVisitor;
    program->accept(printVisitor);
    printVisitor.print();
}
