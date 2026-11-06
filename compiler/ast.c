#include "../include/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// helper function to print the indentation of each LOC
static void print_indent(int indent){
    for(int i = 0 ; i < indent ; i++){
        printf(" ");
    }
}


// function to print the ast tree of nodes 
void ast_print(const ASTNode *node , int indent){


    // if nodes are empty, return null 
    if(!node){
        print_indent(indent);
        printf("null\n");
        return;
    }


    // switch statements to flag each nodes and perform its corresponding functions
    switch(node->kind){

        // if the code is an integer
        // print the indent and flag the code as a integer enum tag
        // print the numeric values being parsed to this function
        case NODE_INT_LITERAL:
           print_indent(indent);
           printf("[Expr: IntLiteral] value=%lld\n" , (long long)node->int_literal.value);
           break;

        // if the code is a return statement
        // print the indent and print the ast tree of the return statement
        // recursively print its returned expression
        case NODE_RETURN_STMT:
           print_indent(indent);
           printf("[Stmt: Return]\n");
           ast_print(node->return_stmt.expr, indent + 1);
           break;

        // if the code is a function declaration
        // prints the function name and the number of body statements
        
        case NODE_FUNCTION_DECL:
           print_indent(indent);
           printf("[Decl: Function] name='%.*s' body_statements=%zu\n",
           (int)node->func_decl.name_len,
           node->func_decl.name,
           node->func_decl.body_count);

           // loop through each body statement and recursively print the ast node
           for(size_t i = 0; i < node->func_decl.body_count ; i ++){
             print_indent(indent + 1);
             printf("Statement[%zu]:\n" , i);
             ast_print(node->func_decl.body[i], indent + 2);
           }
           break;
        
    }
}