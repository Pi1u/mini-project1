#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef struct Command
{
    char** args;
    int count;

    char* inputFile;
    char* outputFile;

    int append;
    int background;
    int connectionType;

    struct Command* next;
} Command;

Command* parse(Token* head);

void freeCommands(Command* root);

#endif