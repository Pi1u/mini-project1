#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "executor.h"
#include "frecency.h"

char prevDirectory[4096] = "";

static int jumpToDir(char* target)
{
    char buffer[4096];
    if (getcwd(buffer, sizeof(buffer)) == NULL)
    {
        perror("Failed to get cwd");
        return -1;
    }
    if (chdir(target) == 0)
    {
        strcpy(prevDirectory, buffer);
        if (getcwd(buffer, sizeof(buffer)) == NULL)
        {
            perror("Failed to get cwd");
            return -1;
        }
        printf("%s\n", buffer);
        recordJump(buffer);
    }
    else
    {
        char* bestMatch = searchFrecency(target);
        if (bestMatch != NULL)
        {
            if (chdir(bestMatch) == 0)
            {
                strcpy(prevDirectory, buffer);
                if (getcwd(buffer, sizeof(buffer)) == NULL)
                {
                    perror("Failed to get cwd");
                    return -1;
                }
                printf("%s\n", buffer);
                recordJump(buffer);
            }
            else
            {
                printf("hop: no such directory\n");
                return -1;
            }
        }
        else
        {
            printf("hop: no such directory\n");
            return -1;
        }
    }
    return 0;
}

void executeHop(Command* command)
{
    if (command -> args[1] == NULL)
    {
        char* homeDir = getenv("HOME");
        if (homeDir != NULL) jumpToDir(homeDir);
        return;
    }
    for (int i = 1; i < command -> count; i++)
    {
        if (strcmp(command -> args[i], "~") == 0)
        {
            char* homeDir = getenv("HOME");
            if (homeDir != NULL) jumpToDir(homeDir);
        }
        else if (strcmp(command -> args[i], "-") == 0)
        {
            if (prevDirectory[0] != '\0') jumpToDir(prevDirectory);
        }
        else jumpToDir(command -> args[i]);
        
    }
}

void executePeek(Command* command) {
    printf("Peek command not yet implemented.\n");
}

void executeLocate(Command* command) {
    printf("Locate command not yet implemented.\n");
}