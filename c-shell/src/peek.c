#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include "intrinsics.h"

static void reverseString(char* str) 
{
    int start = 0;
    int end = strlen(str) - 1;
    while (start < end) 
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void executePeek(Command* command)
{
    int nFlag = 0;
    int rFlag = 0;

    char* files[256];
    int fileCount = 0;

    for (int i = 1; i < command -> count; i++)
    {
        char* arg = command -> args[i];

        if (arg[0] == '-' && strlen(arg) > 1)
        {
            for (size_t j = 1; j < strlen(arg); j++)
            {
                if (arg[j] == 'n') nFlag = 1;
                else if (arg[j] == 'r') rFlag = 1;
                else
                {
                    printf("peek: invalid syntax\n");
                    return;
                }
            }
        }
        else files[fileCount++] = arg;
    }
    if (fileCount == 0) files[fileCount++] = "-";

    int line = 1;
    for (int i = 0; i < fileCount; i++)
    {
        FILE* fp;
        if (strcmp(files[i], "-") == 0) fp = stdin;
        else
        {
            fp = fopen(files[i], "r");
            if (fp == NULL)
            {
                printf("peek: no such file or directory\n");
                continue;
            }
            int fd = fileno(fp);
            struct stat fileStat;

            if (fstat(fd, &fileStat) == 0)
            {
                if (S_ISDIR(fileStat.st_mode))
                {
                    printf("peek: is a directory\n");
                    fclose(fp);
                    continue;
                }
            }
        }

        if (rFlag == 0)
        {
            char buffer[4096];
            int lastChar = '\n';

            while (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
                if (nFlag == 0) printf("%s", buffer);
                else
                {
                    if (buffer[0] == '\n' || (buffer[0] == '\r' && buffer[1] == '\n')) printf("%s", buffer);
                    else printf("%d %s", line++, buffer);
                }

                size_t len = strlen(buffer);
                if (len > 0) lastChar = buffer[len - 1];

            }
            if (fp == stdin) clearerr(stdin);
            if (lastChar != '\n') printf("\n");
            if (fp != stdin) fclose(fp);
        }
        else
        {
            if (fp == stdin)
            {
                int cap = 4096;
                int count = 0;
                char** store = malloc(sizeof(char*) * cap);
                char buffer[4096];
                while (fgets(buffer, sizeof(buffer), fp) != NULL) 
                {
                    int isEmpty = (buffer[0] == '\n' || (buffer[0] == '\r' && buffer[1] == '\n'));
                    if (!isEmpty) line++;
                    store[count++] = strdup(buffer);

                    if (count >= cap)
                    {
                        cap *= 2;
                        char** temp = realloc(store, sizeof(char*) * cap);
                        if (temp == NULL)
                        {
                            perror("peek: realloc failed");
                            for (int i = 0; i < count; i++) free(store[i]);
                            free(store);
                            return;
                        }
                        store = temp;
                    }
                }
                clearerr(stdin);

                int temp = line;
                for (int i = count - 1; i >= 0; i--)
                {
                    int isEmpty = (store[i][0] == '\n' || (store[i][0] == '\r' && store[i][1] == '\n'));
                    if (!nFlag || isEmpty) printf("%s", store[i]);
                    else printf("%d %s", --temp, store[i]);
                    free(store[i]);
                }
                free(store);
            }
            else
            {
                char tempBuffer[4096];

                while (fgets(tempBuffer, sizeof(tempBuffer), fp) != NULL)
                {
                    int isEmpty = (tempBuffer[0] == '\n' || (tempBuffer[0] == '\r' && tempBuffer[1] == '\n'));
                    if (!isEmpty) line++;
                }
                
                int temp = line;

                int fd = fileno(fp);
                off_t fileSize = lseek(fd, 0, SEEK_END);

                if (fileSize == 0) continue;

                int chunkSize = 1024;
                off_t currentPos = fileSize;
                char chunk[1024];

                int leftCap = 1024;
                int leftLen = 0;
                char* leftover = malloc(leftCap);

                while (currentPos > 0)
                {
                    int bytesToRead = (currentPos >= chunkSize) ? chunkSize : currentPos;
                    currentPos -= bytesToRead;
                    lseek(fd, currentPos, SEEK_SET);

                    ssize_t bytesRead = read(fd, chunk, bytesToRead);
                    if (bytesRead < 0) break;

                    for (int i = bytesRead - 1; i >= 0; i--)
                    {
                        if (chunk[i] != '\n')
                        {
                            if (leftLen >= leftCap - 1) 
                            {
                                leftCap *= 2;
                                char* temp = realloc(leftover, leftCap);
                                if (!temp) 
                                { 
                                    perror("peek: realloc failed"); 
                                    exit(1); 
                                }
                                leftover = temp;
                            }
                            if (chunk[i] != '\r') leftover[leftLen++] = chunk[i];
                        }
                        else
                        {
                            leftover[leftLen] = '\0';
                            reverseString(leftover);
                            
                            if (leftLen == 0) printf("\n"); 
                            else 
                            {
                                if (!nFlag) printf("%s\n", leftover);
                                else printf("%d %s\n", --temp, leftover);
                            }
                            leftLen = 0;
                        }
                    }
                }
                if (leftLen > 0)
                {
                    leftover[leftLen] = '\0';
                    reverseString(leftover);
                    
                    if (!nFlag) printf("%s\n", leftover);
                    else printf("%d %s\n", --temp, leftover);
                }
                
                free(leftover);
                fclose(fp);
            }
        }
    }
}