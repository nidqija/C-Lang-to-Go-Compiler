#ifndef CODEGEN_H
#define CODEGEN_H


#include "../include/ast.h"
#include <stdio.h>


// function declaration to generate the go code from parsed ast nodes generated and output file as second argument
void codegen_go(const ASTNode *root , FILE *out);


#endif