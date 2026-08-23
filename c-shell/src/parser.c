#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Command* createCommand()
{
    Command* newCommand = malloc(sizeof(Command));

    if (newCommand == NULL) {
        perror("Failed to allocate Command struct");
        exit(1);
    }

    newCommand -> args = NULL;
    newCommand -> count = 0;
    newCommand -> inputFile = NULL;
    newCommand -> outputFile = NULL;
    newCommand -> append = 0;
    newCommand -> background = 0;
    newCommand -> connectionType = 0;
    newCommand -> next = NULL;

    return newCommand;
}

Command* parse(Token* head)
{
    if (head == NULL) return NULL;

    Command* root = createCommand();
    Command* currentCommand = root;

    int capacity = 8;
    currentCommand -> args = malloc(sizeof(char*) * capacity);
    if (currentCommand -> args == NULL)
    {
        perror("Failed to allocate memory for commands");
        exit(1);
    }

    Token* currentToken = head;

    while (currentToken != NULL)
    {
        if (currentToken -> type == TOKEN_WORD)
        {
            if (currentCommand -> count >= capacity - 1)
            {
                capacity *= 2;
                char **temp = realloc(currentCommand -> args, sizeof(char*) * capacity);
                if (temp == NULL)
                {
                    perror("Failed to reallocate memory for commands");
                    free(currentCommand -> args);
                    exit(1);
                }
                currentCommand -> args = temp;
            }
            currentCommand -> args[currentCommand -> count] = strdup(currentToken -> value);
            currentCommand -> count++;
        }
        
        else if (currentToken -> type == TOKEN_LT)
        {
            currentToken = currentToken -> next;
            if (currentToken == NULL || currentToken -> type != TOKEN_WORD)
            {
                fprintf(stderr, "cshell: syntax error near unexpected token `<'\n");
                return NULL;
            }
            currentCommand -> inputFile = strdup(currentToken -> value);
        }
        else if (currentToken -> type == TOKEN_GT)
        {
            currentToken = currentToken -> next;
            if (currentToken == NULL || currentToken -> type != TOKEN_WORD)
            {
                fprintf(stderr, "cshell: syntax error near unexpected token `>'\n");
                return NULL;
            }
            currentCommand -> outputFile = strdup(currentToken -> value);
            currentCommand -> append = 0;
        }
        else if (currentToken -> type == TOKEN_GTGT)
        {
            currentToken = currentToken -> next;
            if (currentToken == NULL || currentToken -> type != TOKEN_WORD)
            {
                fprintf(stderr, "cshell: syntax error near unexpected token `>>'\n");
                return NULL;
            }
            currentCommand -> outputFile = strdup(currentToken -> value);
            currentCommand -> append = 1;
        }
        else if (currentToken -> type == TOKEN_AMP)
        {
            currentCommand -> background = 1;
            if (currentToken -> next != NULL)
            {
                fprintf(stderr, "cshell: syntax error near unexpected token `&'\n");
                return NULL;
            }
        }
        else if (currentToken -> type == TOKEN_PIPE || currentToken -> type == TOKEN_SEMI)
        {
            if (currentToken -> type == TOKEN_PIPE) currentCommand->connectionType = 1;
            else currentCommand -> connectionType = 2;
            
            Command* newCommmand = createCommand();
            currentCommand -> next = newCommmand;
            currentCommand = newCommmand;

            capacity = 8;
            currentCommand -> args = malloc(sizeof(char*) * capacity);
            if (currentCommand -> args == NULL)
            {
                perror("Failed to allocate memory for commands");
                exit(1);
            }
        }
        
        currentToken = currentToken -> next;
    }
    currentCommand -> args[currentCommand -> count] = NULL;
    return root;
}

void freeCommands(Command* root)
{
    Command* current = root;
    Command* temp;

    while (current != NULL)
    {
        temp = current -> next;
        for (int i = 0; i < current -> count; i++)
        {
            free(current -> args[i]);
        }
        free(current -> args);
        free(current -> inputFile);
        free(current -> outputFile);
        free(current);
        current = temp;
    }
}