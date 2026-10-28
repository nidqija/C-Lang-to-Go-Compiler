#include <stdio.h>
#include "lexer.h"


// function to initialize the lexer with the source code


// lexer is required as it is a pointer to the lexer struct, 
// and source is the source code to be lexed
// lexer is a pointer to the lexer struct, which is used to 
// keep track of the current position in the source code, as well as the current line and column numbers. 
void lexer_init(Lexer* lexer , const char *source){

    // init the default values for the lexer struct
    lexer->source = source;
    lexer->cursor = 0;
    lexer->line = 1;
    lexer->col = 1;
}


// function to peek at the current character in
// the source code without advancing the cursor
// this is to look at the current character in the source code without advancing the cursor, 
// which is useful for checking if the current character is a whitespace or a comment.

static char peek(Lexer *lexer){

    // return the current character in the source code without advancing the cursor
    return lexer->source[lexer->cursor];
}

// function to advance the cursor to the next character in the source code
// this is to move the cursor to the next character in the source code,
// which is useful for skipping over whitespace and comments, as well as for reading tokens from the source code.
static char advance(Lexer *lexer){

    // get the current character in the source code and advance the cursor
    char c = lexer->source[lexer->cursor++];

    // if the current character is a newline, increment the line number and reset the column number to 1
    if (c == '\n'){
        lexer->line++;
        lexer->col= 1;

    } else {
        // if the current character is not a newline, increment the column number
        lexer->col++;
    }
}

static void skip_whitespace_and_comments(Lexer *lexer){
    while(1){
        char c = peek(lexer);
        if (c == ' ' || c == "\t" || c == "\r" || c == "\n"){
            advance(lexer);
        } else if (c == "/" && lexer->source[lexer->cursor + 1] == '/') {
            while(peek(lexer) != '\0' && peek(lexer) != '\n'){
                advance(lexer);
            }
        } else {
            break;
        }
    }
}

// function to create a token from the current position in the source code
static Token make_token(Lexer *lexer , TokenType type , size_t start_offset , int line , int col){
  
    Token token;
    token.tokenType = type;
    token.start = &lexer->source[start_offset];
    token.length = &lexer->source[lexer->cursor] - token.start;
    token.line = line;
    token.col = col;
    return token;
}


// function to return the next token from the source code
Token lexer_next(Lexer *lexer){
    skip_whitespace_and_comments(lexer);

    int start_line = lexer->line;
    int start_col = lexer->col;
    size_t start_offset = lexer->cursor;
    char c = advance(lexer);

    if( c == "\0"){
        lexer->cursor--;
        return make_token(lexer , TOKEN_EOF , start_offset , start_line , start_col);
    }


    switch(c){
        case '(':
            return make_token(lexer , TOKEN_LPAREN , start_offset , start_line , start_col);

        case ')':
            return make_token(lexer , TOKEN_RPAREN , start_offset , start_line , start_col);

        case '{':
            return make_token(lexer , TOKEN_LBRACE , start_offset , start_line , start_col);

        case '}':
            return make_token(lexer , TOKEN_RBRACE , start_offset , start_line , start_col);

        case ';':
            return make_token(lexer , TOKEN_SEMICOLON , start_offset , start_line , start_col);
    }

    if(isdigit((unsigned char)c)){
        while(isdigit((unsigned char)peek(lexer))){
            advance(lexer);
        }
        return make_token(lexer , TOKEN_INT_LIT , start_offset , start_line , start_col);
    }

    if(isalpha((unsigned char)c) || c == '_'){
        // TBA
    }


}