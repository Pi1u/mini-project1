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
    newCommand -> next = NULL;

    return newCommand;
}

Command* parse(Token* head)
{
    Token* temp = head;
    TokenType cT = head -> type;
    char* cV = strdup(head -> value);

    while (temp != NULL)
    {
        char** buffer;
        int count = 0;
        if (cT == TOKEN_LT)
        {
            temp = temp -> next;
            if (temp == NULL || temp -> type != TOKEN_WORD)
            {
                perror("c-shell: Invalid syntax");
                return NULL;
            }

        }
    }
    

}