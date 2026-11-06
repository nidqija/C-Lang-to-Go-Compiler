#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"
#include "arena.h"
#include "ast.h"


// struct to init the object and instance for lexer , current token and arena ( memory management fields )
typedef struct {
    Lexer *lexer;
    Token current;
    Arena *arena;

} Parser;


// init function to start parsing the LOC
void parser_init(Parser *parser , Lexer *lexer, Arena *arena);

// init function to parse the LOC for translation units
ASTNode *parser_parse_translation_unit(Parser *parser);




#endif /* PARSER_H */