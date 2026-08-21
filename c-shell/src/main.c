#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"
#include "lexer.h"

int main(void) {
    char input[1024];
    while (1)
    {
        displayPrompt();

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';
        Token* head = tokenize(input);
        Token* temp = head;

        while (temp != NULL)
        {
            printf("Token Type: %d | Token Value: [%s]\n", temp->type, temp->value);
            temp = temp -> next;
        }
        freeTokens(head);
    }
    return 0;
}