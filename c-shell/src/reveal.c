#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "intrinsics.h"

int compare_pointers(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    return strcmp(str1, str2);
}

void revealRecursive(const char* basePath, const char* prefix, int aFlag)
{
    DIR* dir = opendir(basePath);
    if (dir == NULL)
    {
        printf("reveal: no such directory\n");
        return;
    }

    int capacity = 1024;
    int count = 0;

    char** entries = malloc(sizeof(char*) * capacity); 
    if (entries == NULL)
    {
        perror("reveal: malloc failed");
        closedir(dir);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!aFlag && entry -> d_name[0] == '.') continue;
        else entries[count++] = strdup(entry -> d_name);
        if (count >= capacity)
        {
            capacity *= 2;
            char** temp = realloc(entries, capacity * sizeof(char*));
            if (temp == NULL)
            {
                perror("reveal: realloc failed");
                for (int i = 0; i < count; i++) free(entries[i]);
                free(entries);
                closedir(dir);
                return;
            }
            entries = temp;
        }
    }
    closedir(dir);
    qsort(entries, count, sizeof(char*), compare_pointers);

    for (int i = 0; i < count; i++)
    {
        struct stat pathStat;
        char fullPath[4096];
        sprintf(fullPath, "%s/%s", basePath, entries[i]);
        
        if (stat(fullPath, &pathStat) == 0)
        {
            if (S_ISDIR(pathStat.st_mode))
            {
                if (strcmp(entries[i], ".") != 0 && strcmp(entries[i], "..") != 0)
                {
                    printf("%s%s/\n", prefix, entries[i]);
                    char newPrefix[4096];
                    sprintf(newPrefix, "%s%s/", prefix, entries[i]);
                    revealRecursive(fullPath, newPrefix, aFlag);
                }
            }
            else
            {
                // Print regular file
                printf("%s%s\n", prefix, entries[i]);
            }
        }
        free(entries[i]);
    }
    free(entries);
}

void executeReveal(Command* command)
{
    int aFlag = 0;
    int tFlag = 0;
    char* target = NULL;

    for (int i = 1; i < command->count; i++)
    {
        char* arg = command->args[i];

        if (arg[0] == '-' && strlen(arg) > 1)
        {
            for (size_t j = 1; j < strlen(arg); j++)
            {
                if (arg[j] == 'a') aFlag = 1;
                else if (arg[j] == 't') tFlag = 1;
                else
                {
                    printf("reveal: invalid syntax\n");
                    return;
                }
            }
        }
        else 
        {
            if (target != NULL) 
            {
                printf("reveal: invalid syntax\n");
                return;
            }
            target = arg;
        }
    }

    if (target == NULL) target = ".";
    else if (strcmp(target, "~") == 0) 
    {
        target = getenv("HOME");
        if (target == NULL) return; 
    }
    else if (strcmp(target, "-") == 0)
    {
        if (prevDirectory[0] != '\0') target = prevDirectory;
        else
        {
            printf("reveal: no such directory\n");
            return;
        }
    }

    if (tFlag == 1) revealRecursive(target, "", aFlag);
    else
    {
        DIR* dir = opendir(target);
        if (dir == NULL)
        {
            printf("reveal: no such directory\n");
            return;
        }

        int capacity = 1024;
        int count = 0;

        char** entries = malloc(sizeof(char*) * capacity); 
        if (entries == NULL)
        {
            perror("reveal: malloc failed");
            closedir(dir);
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (!aFlag && entry -> d_name[0] == '.') continue;
            else entries[count++] = strdup(entry -> d_name);
            if (count >= capacity)
            {
                capacity *= 2;
                char** temp = realloc(entries, capacity * sizeof(char*));
                if (temp == NULL)
                {
                    perror("reveal: realloc failed");
                    for (int i = 0; i < count; i++) free(entries[i]);
                    free(entries);
                    closedir(dir);
                    return;
                }
                entries = temp;
            }
        }
        closedir(dir);
        qsort(entries, count, sizeof(char*), compare_pointers);

        for (int i = 0; i < count; i++)
        {
            printf("%s\n", entries[i]);
            free(entries[i]);
        }
        free(entries);
    }
}