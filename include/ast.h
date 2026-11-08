#ifndef AST_H
#define AST_H


#include <stdio.h>
#include <stdint.h>


// enum for detecting integers , return statements and function declarations
typedef enum {
    NODE_INT_LITERAL,
    NODE_STRING_LITERAL,
    NODE_RETURN_STMT,
    NODE_FUNCTION_DECL,
    NODE_EXPR_STMT,
    NODE_CALL_STMT
} NodeKind;


typedef struct ASTNode ASTNode;

struct ASTNode{
    NodeKind kind;

    // define a family of structs under a union
    union {
        
        // struct for detecting integers 
        struct {
            int64_t value;
        }int_literal;

        struct {
            const char *value;
            size_t length;

        } string_literal;


        struct {
            const char *name;
            size_t name_len;
            ASTNode **args;
            size_t arg_count;

        } call_expr;

    // struct for detecting return statements
    struct {
        ASTNode *expr;
    } return_stmt;


    // struct for detecting function declarations;
    struct {
        const char *name;
        size_t name_len;
        ASTNode **body;
        size_t body_count;
    } func_decl;
    };
};

// helper function to print the ast tree nodes and indexing helper
void ast_print(const ASTNode *node , int indent);


#endif