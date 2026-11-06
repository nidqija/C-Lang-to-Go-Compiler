#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>

// function to move to the next token that is being parsed by the lexer
static void advance_token(Parser *parser){
    parser->current = lexer_next(parser->lexer);
}


/*static int match(Parser *parser , TokenType type){
    if(parser->current.tokenType == type){
        advance_token(parser);
        return 1;
    }

    return 0;
} */

// function to verify the current token matches type , advances if matched , or reports a syntax error and aborts
static void expect(Parser *parser , TokenType type , const char *err_msg){

    // if the parsed token does not match with the expected type , output a syntax error indicating a type mismatch
    if(parser->current.tokenType != type){
        fprintf(stderr, "Syntax Error [%d:%d]: %s (got %s)\n",
                parser->current.line,
                parser->current.col,
                err_msg,
                token_type_name(parser->current.tokenType));
        exit(EXIT_FAILURE);
    }

    // if it matches , advance to the next tokenized syntax
    advance_token(parser);

}


// function to init the parser wiht lexer and arena reference , and primes the first token
void parser_init(Parser *parser , Lexer *lexer , Arena *arena){
    parser->lexer = lexer;
    parser->arena = arena;
    advance_token(parser);
}


// function to parse each expression from the code that is being tokenized
static ASTNode *parse_expression(Parser *parser){
    if(parser->current.tokenType == TOKEN_INT_LIT){
        ASTNode *node = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        node->kind = NODE_INT_LITERAL;
        node->int_literal.value = strtoll(parser->current.start , NULL , 10);
        advance_token(parser);
        return node;
    }

    fprintf(stderr, "Syntax Error [%d:%d]: Expected expression\n",
            parser->current.line, parser->current.col);
    exit(EXIT_FAILURE);
}

// function to parse the statement and returns statement AST Node
static ASTNode *parse_statement(Parser *parser){
    if(parser->current.tokenType == TOKEN_RETURN_KW){
        advance_token(parser);
        ASTNode *expr = parse_expression(parser);
        expect(parser , TOKEN_SEMICOLON , "Expected ';' after a return value");


        ASTNode *stmt = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        stmt->kind = NODE_RETURN_STMT;
        stmt->return_stmt.expr = expr;
        return stmt;
    }

    fprintf(stderr, "Syntax Error [%d:%d]: Expected statement\n",
            parser->current.line, parser->current.col);
    exit(EXIT_FAILURE);
}


// function to parse an entire c function declaration and returns the root AST node
ASTNode *parser_parse_translation_unit(Parser *parser){

    expect(parser , TOKEN_INT_KW , "expected return type 'int");

    Token name_token = parser->current;
    expect(parser , TOKEN_IDENT , "expected function name");

    expect(parser , TOKEN_LPAREN , "Expected '(' after function name");

    if(parser->current.tokenType == TOKEN_VOID_KW){
        advance_token(parser);
    }

    expect(parser , TOKEN_RPAREN , "Expected ')' after parameter list");

    expect(parser , TOKEN_LBRACE , "Expected '{' to start function body");

    size_t capacity = 4;
    size_t count = 0;

    ASTNode **statements = (ASTNode  **)arena_alloc(parser->arena , sizeof(ASTNode *) *capacity);

    while(parser->current.tokenType != TOKEN_RBRACE && parser->current.tokenType != TOKEN_EOF){
        if(count >= capacity){
            size_t new_capacity = capacity * 2;
            ASTNode **new_statements = (ASTNode **)arena_alloc(parser->arena , sizeof(ASTNode*)* new_capacity);

            for(size_t i = 0; i < count ; i++){
                new_statements[i] = statements[i];

            }

            statements = new_statements;
            capacity = new_capacity;

        }

        statements[count++]  = parse_statement(parser);
    }

    expect(parser , TOKEN_RBRACE , "Expected '}' to close the function body" );
    
    ASTNode *func = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
    func->kind = NODE_FUNCTION_DECL;
    func->func_decl.name = name_token.start;
    func->func_decl.name_len = name_token.length;
    func->func_decl.body = statements;
    func->func_decl.body_count = count;

    return func;



}