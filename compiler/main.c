#include "../include/arena.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>


static char* read_entire_file(const char *path){
    FILE *file = fopen(path, "rb");

    if(!file){
        fprintf(stderr, "Error: Could not open file %s\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = (char *)malloc(length + 1);

    if(!buf){
        perror("Error: Could not allocate memory for file buffer");
        fclose(file);
        return NULL;
    }

    fread(buf, 1, length, file);

    buf[length] = '\0'; // Null-terminate the buffer

    fclose(file);
    return buf;



}


int main(int argc , char **argv){
    if(argc < 2){
        
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *source = read_entire_file(argv[1]);

    if(!source){
        return EXIT_FAILURE;
    }

    printf("Source code:\n%s\n", source);
    printf("Tokenizing source code...\n");

    Lexer lexer;
    lexer_init(&lexer, source);

    Token token;
    do {
        token = lexer_next(&lexer);
        printf("Token: Type=%d, Start='%.*s', Length=%zu, Line=%d, Col=%d\n",
               token.tokenType, (int)token.length, token.start, token.length, token.line, token.col);

    }while (token.tokenType != TOKEN_EOF);

    free(source);
    return EXIT_SUCCESS;
}