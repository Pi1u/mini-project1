#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include "intrinsics.h"

void executeCommands(Command* root)
{
    Command* curr = root;
    while (curr != NULL)
    {
        if (curr -> args == NULL || curr -> args[0] == NULL) {
            curr = curr->next;
            continue;
        }

        // --- 2. THE INTERCEPTOR ---
        if (strcmp(curr -> args[0], "hop") == 0) {
            executeHop(curr);
            curr = curr->next;
            continue;
        }
        else if (strcmp(curr -> args[0], "reveal") == 0) {
            executeReveal(curr);
            curr = curr->next;
            continue;
        }
        else if (strcmp(curr -> args[0], "peek") == 0) {
            executePeek(curr);
            curr = curr->next;
            continue;
        }
        else if (strcmp(curr -> args[0], "locate") == 0) {
            executeLocate(curr);
            curr = curr->next;
            continue;
        }

        pid_t pid = fork();
        
        if (pid < 0) {
            perror("cshell: fork failed");
            break; 
        } 
        else if (pid == 0) {
            execvp(curr->args[0], curr->args);
            perror("cshell");
            exit(1); 
        } 
        else {
            int status;
            waitpid(pid, &status, 0);
        }

        curr = curr->next;
    }   
}