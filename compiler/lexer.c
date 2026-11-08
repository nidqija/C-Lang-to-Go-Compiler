#include <stdio.h>
#include "../include/lexer.h"
#include <ctype.h>
#include <string.h>


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

    return c;
}

static void skip_whitespace_and_comments(Lexer *lexer){
    while(1){
        char c = peek(lexer);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' ){
            advance(lexer);
        } else if (c == '/' && lexer->source[lexer->cursor + 1] == '/') {
            while(peek(lexer) != '\0' && peek(lexer) != '\n'){
                advance(lexer);
            }
            
        } else if (c == '#'){
            while(peek(lexer) != '\0' && peek(lexer) != '\n'){
                advance(lexer);
            }
        }
        
        else {
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

    if( c == '\0'){
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


    // check if current tokenized lexer has double quote
    // this is for detecting any strings from the input code
    if(c == '"'){
        while(peek(lexer) != '"' && peek(lexer) != '\0'){
            if(peek(lexer) == '\\'){
                advance(lexer);
            }

            advance(lexer);
        }

        if (peek(lexer) == '"'){
            advance(lexer);
        }

        return make_token(lexer , TOKEN_STRING_LIT , start_offset , start_line , start_col);
    }

    if(isdigit((unsigned char)c)){
        while(isdigit((unsigned char)peek(lexer))){
            advance(lexer);
        }
        return make_token(lexer , TOKEN_INT_LIT , start_offset , start_line , start_col);
    }

    // check if the current character is a letter or an underscore, which indicates the start of an identifier or a keyword
    if(isalpha((unsigned char)c) || c == '_'){

        // loop through the source code until we reach a character that is not a letter, digit, or underscore
        while(isalnum((unsigned char)peek(lexer)) || peek(lexer) == '_'){

            // if the current character is a letter, digit, or underscore, advance the cursor to the next character
            advance(lexer);
        }
        

        // calculate the length of the identifier or keyword by subtracting the start offset from the current cursor position
        size_t length = lexer->cursor - start_offset;

        // get a ponter to the start of the identifier or keyword in the source code
        // this is important because we need to compare the identifier or keyword to 
        // the list of keywords in the language to determine its token type
        // we will parse the token type of the identifier or keyword by comparing it to the list of keywords in the language,
        // and if it matches a keyword, we will return the corresponding token type.
        const char *identifier = &lexer->source[start_offset];



        // if the length of the identifier is 3 and the identifier is "int", return a token of type TOKEN_INT_KW
        if(length == 3 && strncmp(identifier, "int" , 3) == 0){
            return make_token(lexer , TOKEN_INT_KW , start_offset , start_line , start_col);
        }

        // if the length of the identifier is 4 and the identifier is "void", return a token of type TOKEN_VOID_KW
        if(length == 4 && strncmp(identifier, "void" , 4) == 0){
            return make_token(lexer , TOKEN_VOID_KW , start_offset , start_line , start_col);
        }

        // if the length of the identifier is 6 and the identifier is "return", return a token of type TOKEN_RETURN_KW
        if (length == 6 && strncmp(identifier, "return" , 6) == 0){
            return make_token(lexer , TOKEN_RETURN_KW , start_offset , start_line , start_col);
        }

        // if the identifier does not match any of the keywords, return a token of type TOKEN_IDENT
        return make_token(lexer , TOKEN_IDENT , start_offset , start_line , start_col);

        
    }

    return make_token(lexer, TOKEN_UNKNOWN , start_offset, start_line , start_col);

   
}

 const char* token_type_name(TokenType type){


        switch(type){
            case TOKEN_EOF:
                return "EOF";

            case TOKEN_INT_KW:
                return "KEYWORD(int)";

            case TOKEN_RETURN_KW:
                return "KEYWORD(return)";

            case TOKEN_VOID_KW:
                return "KEYWORD(void)";

            case TOKEN_IDENT:
                return "IDENTIFIER";

            case TOKEN_INT_LIT:
                return "INTEGER_LITERAL";

            case TOKEN_STRING_LIT:
                return "STRING_LITERAL";

            case TOKEN_LPAREN:
                return "LPAREN";
            
            case TOKEN_RPAREN:
                return "RPAREN";

            case TOKEN_RBRACE:
                return "RBRACE";

            case TOKEN_LBRACE:
                return "LBRACE";

            case TOKEN_SEMICOLON:
                return "SEMICOLON";

            default:
                return "UNKNOWN";

            

            
        }
        

    }


    void lexer_free(Lexer *lexer){
        (void)lexer;
    }