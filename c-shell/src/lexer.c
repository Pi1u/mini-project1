#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef enum
{
    STATE_NORMAL,
    STATE_IN_SQ, //Single Quotes
    STATE_IN_DQ  //Double Quotes
} LexerState;

void addToken(Token** head, Token** tail, TokenType type, char *value)
{
    Token* newToken = malloc(sizeof(Token));
    if (newToken == NULL)
    {
        perror("Failed to allocate token");
        exit(1);
    }

    newToken -> type = type;

    newToken->value = malloc(strlen(value) + 1);
    strcpy(newToken->value, value);

    newToken -> next = NULL;
    
    if ((*head) == NULL) (*head) = newToken;
    else (*tail) -> next = newToken;

    (*tail) = newToken;
}

void freeTokens(Token* head)
{
    Token* current = head;
    Token* temp;

    while (current != NULL)
    {
        temp = current -> next;
        free(current -> value);
        free(current);
        current = temp;
    }
}

Token *tokenize(char* input)
{
    Token *head = NULL;
    Token *tail = NULL;

    LexerState state = STATE_NORMAL;

    char buffer[1024];
    int buf_idx = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = input[i];
        
        if (state == STATE_NORMAL) {
            if (c == ' ' || c == '\t')
            {
                if (buf_idx > 0)
                {
                    buffer[buf_idx] = '\0';
                    addToken(&head, &tail, TOKEN_WORD, buffer);
                    buf_idx = 0;
                }
            }
            else if (c == '\'')
            {
                state = STATE_IN_SQ;
            }
            else if (c == '"')
            {
                state = STATE_IN_DQ;
            }

            else if (c == '\\')
            {
                if (input[i + 1] != '\0') buffer[buf_idx++] = input[++i];
                else
                {
                    printf("c-shell: invalid syntax\n");
                    return NULL;
                }
            }
            else if (c == '|')
            {
                if (buf_idx > 0)
                {
                    buffer[buf_idx] = '\0';
                    addToken(&head, &tail, TOKEN_WORD, buffer);
                    buf_idx = 0;
                }
                addToken(&head, &tail, TOKEN_PIPE, "|");
            }
            else if (c == '&')
            {
                if (buf_idx > 0)
                {
                    buffer[buf_idx] = '\0';
                    addToken(&head, &tail, TOKEN_WORD, buffer);
                    buf_idx = 0;
                }
                addToken(&head, &tail, TOKEN_AMP, "&");
            }
            else if (c == '<')
            {
                if (buf_idx > 0)
                {
                    buffer[buf_idx] = '\0';
                    addToken(&head, &tail, TOKEN_WORD, buffer);
                    buf_idx = 0;
                }
                addToken(&head, &tail, TOKEN_LT, "<");
            }
            else if (c == '>')
            {
                if (buf_idx > 0)
                {
                    buffer[buf_idx] = '\0';
                    addToken(&head, &tail, TOKEN_WORD, buffer);
                    buf_idx = 0;
                }

                if (input[i + 1] == '>')
                {
                    addToken(&head, &tail, TOKEN_GTGT, ">>");
                    i++;
                }
                else addToken(&head, &tail, TOKEN_GT, ">");
            }


            else buffer[buf_idx++] = c;
        } 
        else if (state == STATE_IN_SQ)
        {
            if (c == '\'') state = STATE_NORMAL;
            else buffer[buf_idx++] = c;
        } 
        else if (state == STATE_IN_DQ)
        {
            if (c == '"') state = STATE_NORMAL;
            else if (c == '\\')
            {
                if (input[i + 1] != '\0') buffer[buf_idx++] = input[++i];
                else
                {
                    printf("c-shell: invalid syntax\n");
                    return NULL;
                }
            }
            else buffer[buf_idx++] = c;
        }
    }

    if (buf_idx > 0)
    {
        buffer[buf_idx] = '\0';
        addToken(&head, &tail, TOKEN_WORD, buffer);
        buf_idx = 0;
    }

    //Lexical Error Checking
    if (state != STATE_NORMAL)
    {
        printf("c-shell: invalid syntax\n");
        freeTokens(head);
        return NULL;
    }
    return head;
}