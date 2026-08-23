#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_WORD,     // Regular commands
    TOKEN_PIPE,     // |
    TOKEN_AMP,      // &
    TOKEN_SEMI,     // ;
    TOKEN_LT,       // <
    TOKEN_GT,       // >
    TOKEN_GTGT      // >>
} TokenType;

//Token
typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;
} Token;

void freeTokens(Token* head);

Token* tokenize(char* input);

#endif