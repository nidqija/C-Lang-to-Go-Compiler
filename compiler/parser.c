#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>

// function to move to the next token that is being parsed by the lexer
static void advance_token(Parser *parser){
    parser->current = lexer_next(parser->lexer);
}

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

// Forward declarations to allow mutual recursion across precedence levels
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_multi_divide(Parser *parser);
static ASTNode *parse_primary(Parser *parser);

static ASTNode *parse_expression(Parser *parser){
    ASTNode *left = parse_multi_divide(parser);


    while(parser->current.tokenType == TOKEN_PLUS || parser->current.tokenType == TOKEN_MINUS || parser->current.tokenType == TOKEN_LT){
        TokenType op = parser->current.tokenType;
        advance_token(parser);

        ASTNode *right = parse_multi_divide(parser);

        ASTNode *bin = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        bin->kind = NODE_BINARY_EXPR;
        bin->binary_expr.op = op;
        bin->binary_expr.left = left;
        bin->binary_expr.right = right;


        left = bin;
    }

    return left;
}



// function to parse multiplication and division operation , 
// we put this in another function as this requires more priority 
// according to PEMDAS rules
static ASTNode *parse_multi_divide(Parser *parser){
    ASTNode *left = parse_primary(parser);


    while(parser->current.tokenType == TOKEN_MULTI || parser->current.tokenType == TOKEN_DIVIDE){
        TokenType op = parser->current.tokenType;
        advance_token(parser);

        ASTNode *right = parse_primary(parser);

        ASTNode *bin = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        bin->kind = NODE_BINARY_EXPR;
        bin->binary_expr.op = op;
        bin->binary_expr.left = left;
        bin->binary_expr.right = right;

        left = bin;


    }

    return left;
}


// function to parse each expression from the code that is being tokenized
static ASTNode *parse_primary(Parser *parser){
    if(parser->current.tokenType == TOKEN_INT_LIT){
        ASTNode *node = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        node->kind = NODE_INT_LITERAL;
        node->int_literal.value = strtoll(parser->current.start , NULL , 10);
        advance_token(parser);
        return node;
    }

    // check if parsed token is a string literal
    if(parser->current.tokenType == TOKEN_STRING_LIT){
        ASTNode *node = (ASTNode *)arena_alloc(parser->arena, sizeof(ASTNode));
        node->kind = NODE_STRING_LITERAL;
        node->string_literal.value = parser->current.start;
        node->string_literal.length = parser->current.length;
        advance_token(parser);
        return node;
    }


    if(parser->current.tokenType == TOKEN_IDENT){
        Token id_token = parser->current;
        advance_token(parser);


        if(parser->current.tokenType == TOKEN_INC){
            advance_token(parser);
            ASTNode *inc = (ASTNode *)arena_alloc(parser->arena, sizeof(ASTNode));
            inc->kind = NODE_INC_EXPR;
            inc->inc_expr.name = id_token.start;
            inc->inc_expr.name_len = id_token.length;
            return inc;
        }

        

        if(parser->current.tokenType == TOKEN_LPAREN){
            advance_token(parser);
            size_t capacity = 4;
            size_t arg_count = 0;

            ASTNode **args = (ASTNode **) arena_alloc(parser->arena , sizeof(ASTNode *) * capacity);

            while(parser->current.tokenType != TOKEN_RPAREN && parser->current.tokenType != TOKEN_EOF){
                if(arg_count >= capacity){
                    size_t new_capacity = capacity * 2;
                    ASTNode **new_args = (ASTNode **)arena_alloc(parser->arena , sizeof(ASTNode *) * new_capacity);

                    for (size_t i = 0; i < arg_count ; i++){
                        new_args[i] = args[i];
                      }
                        args = new_args;
                        capacity = new_capacity;
                    }
                    args[arg_count++] = parse_expression(parser);
                }

                expect(parser, TOKEN_RPAREN, "Expected ')' after function arguments");

                ASTNode *node = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));

                node->kind = NODE_CALL_STMT;
                node->call_expr.name = id_token.start;
                node->call_expr.name_len = id_token.length;
                node->call_expr.args = args;
                node->call_expr.arg_count = arg_count;
                return node;
            
        }

        ASTNode *var_node = (ASTNode *)arena_alloc(parser->arena, sizeof(ASTNode));
        var_node->kind = NODE_VAR_REF;
        var_node->var_ref.name = id_token.start;
        var_node->var_ref.name_len = id_token.length;
        return var_node;
    }


   

    fprintf(stderr, "Syntax Error [%d:%d]: Expected expression\n",
            parser->current.line, parser->current.col);
    exit(EXIT_FAILURE);
}









/*static int match(Parser *parser , TokenType type){
    if(parser->current.tokenType == type){
        advance_token(parser);
        return 1;
    }

    return 0;
} */




// function to init the parser wiht lexer and arena reference , and primes the first token
void parser_init(Parser *parser , Lexer *lexer , Arena *arena){
    parser->lexer = lexer;
    parser->arena = arena;
    advance_token(parser);
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

    if(parser->current.tokenType == TOKEN_INT_KW){
        advance_token(parser);
        Token name_token = parser->current;
        expect(parser , TOKEN_IDENT , "Expected variable after 'int'");

        expect(parser , TOKEN_ASSIGN , "Expected '=' after variable name");

        ASTNode *init_expr = parse_expression(parser);

        expect(parser , TOKEN_SEMICOLON, "Expected ';' after variable declaration");

        ASTNode *node = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        node->kind = NODE_VAR_DECL;
        node->var_decl.name = name_token.start;
        node->var_decl.name_len = name_token.length;
        node->var_decl.init = init_expr;
        return node;
    }


     // check if parser detect the current token type is a for statement
    if(parser->current.tokenType == TOKEN_FOR_KW){

        // advance to next token to analyze syntax in the for loop
        advance_token(parser);
        expect(parser, TOKEN_LPAREN , "Expected '(' after 'for'");
        ASTNode *init = parse_statement(parser);
        

        ASTNode *cond = parse_expression(parser);
        expect(parser , TOKEN_SEMICOLON, "Expected ';' after for clauses");

        ASTNode *post = parse_expression(parser);

        expect(parser, TOKEN_RPAREN , "Expected ')' after for clauses");

        expect(parser ,TOKEN_LBRACE , "Expected '{' to start loop body");


        size_t capacity = 4;
        size_t count = 0;




        ASTNode **body = (ASTNode **)arena_alloc(parser->arena, sizeof(ASTNode *) * capacity);

        while(parser->current.tokenType != TOKEN_RBRACE && parser->current.tokenType != TOKEN_EOF){
            if(count >= capacity ){
                size_t new_capacity = capacity * 2;

                ASTNode **new_body = (ASTNode **) arena_alloc(parser->arena , sizeof(ASTNode *) * new_capacity);

                for(size_t i = 0 ; i < count; i++){
                    new_body[i] = body[i];
                }

                body = new_body;
                capacity = new_capacity;
            }

            body[count++] = parse_statement(parser);
        }

        expect(parser , TOKEN_RBRACE , "Expected '}' to close loop body");


        ASTNode *node = (ASTNode *)arena_alloc(parser->arena , sizeof(ASTNode));
        node->kind = NODE_FOR_STMT;
        node->for_stmt.init = init;
        node->for_stmt.cond = cond;
        node->for_stmt.post = post;
        node->for_stmt.body = body;
        node->for_stmt.body_count = count;

        return node;

        
    }

    ASTNode *expr = parse_expression(parser);
    expect(parser, TOKEN_SEMICOLON , "Expected ';' after statement");
    ASTNode *stmt = (ASTNode *)arena_alloc(parser->arena, sizeof(ASTNode));
    stmt->kind = NODE_EXPR_STMT;
    stmt->return_stmt.expr = expr;
    return stmt;

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



