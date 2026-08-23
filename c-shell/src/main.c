#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"
#include "lexer.h"
#include "parser.h"

void printAST(Command* root) {
    Command* curr = root;
    int cmd_num = 1;
    while (curr != NULL) {
        printf("--- Command %d ---\n", cmd_num++);
        for (int i = 0; i < curr->count; i++) {
            printf("  Arg[%d]: %s\n", i, curr->args[i]);
        }
        if (curr->inputFile) printf("  Input: %s\n", curr->inputFile);
        if (curr->outputFile) {
            printf("  Output: %s ", curr->outputFile);
            printf(curr->append ? "(APPEND)\n" : "(TRUNCATE)\n");
        }
        if (curr->background) printf("  Background: YES\n");
        printf("  Connection: %d (1=PIPE, 2=SEMI, 0=NONE)\n", curr->connectionType);
        curr = curr->next;
    }
}

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

        Command* root = parse(head);
        printAST(root);
        freeCommands(root);
        // while (temp != NULL)
        // {
        //     printf("Token Type: %d | Token Value: [%s]\n", temp->type, temp->value);
        //     temp = temp -> next;
        // }
        
        freeTokens(head);
    }
    return 0;
}