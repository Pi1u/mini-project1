#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"

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

        if (strlen(input) > 0) {
            printf("You typed: '%s'\n", input);
        }
        

    }
    return 0;
}