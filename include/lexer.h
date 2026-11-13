#ifndef LEXER_H
#define LEXER_H
#include <stddef.h> // we need this library to put size_t



// enum to flag different types of tokens
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_INT_KW,       // "int"
    TOKEN_RETURN_KW,    // "return"
    TOKEN_VOID_KW,      // "void"
    TOKEN_IDENT,        // identifiers (e.g., function names)
    TOKEN_INT_LIT,      // numbers (e.g., 42)
    TOKEN_STRING_LIT,   // for strings such as "Hello World"
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_SEMICOLON  ,   // ;
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTI,
    TOKEN_DIVIDE,
    TOKEN_FOR_KW,
    TOKEN_ASSIGN,
    TOKEN_LT,
    TOKEN_INC,
    TOKEN_COMMA,
    TOKEN_UNKNOWN,
  
    
} TokenType;

// struct for token
typedef struct {
    TokenType tokenType;
    const char *start;
    size_t length;
    int line;
    int col;
} Token;


// struct for lexer
typedef struct {
    const char *source;
    size_t cursor;
    int line;
    int col;
} Lexer;


// function to create a lexer session 
void lexer_init(Lexer* lexer , const char *source);

// function to return the next token from the source code
Token lexer_next(Lexer *lexer);

// function to free the lexer session
void lexer_free(Lexer *lexer);

// function to get string representation of token type
const char* token_type_name(TokenType type);


#endif