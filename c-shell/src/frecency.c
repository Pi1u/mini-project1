#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "intrinsics.h"

#define MAX_RECORDS 10000
static FrecencyRecord db[MAX_RECORDS];
static int dbSize = 0;

char* searchFrecency(const char* query)
{
    char dpPath[4096];
    snprintf(dpPath, sizeof(dpPath), "%s/.hop_frecency", getenv("HOME"));

    dbSize = 0;

    // --- STEP 1: READ EXISTING DATA ---
    FILE* file = fopen(dpPath, "r");
    if (file != NULL)
    {
        char line[5000];
        while (fgets(line, sizeof(line), file))
        {
            if (sscanf(line, "%[^,],%d,%ld", db[dbSize].path, &db[dbSize].frequency, &db[dbSize].recency) == 3) dbSize++;
        }
        fclose(file);
    }

    time_t now = time(NULL);
    int bestScore = -1;
    char* bestPath = NULL;

    for (int i = 0; i < dbSize; i++)
    {
        if (strstr(db[i].path, query) != NULL)
        {
            double diff = difftime(now, db[i].recency);
            int score = 0;

            if (diff >= 604800) score = db[i].frequency * 1;
            else if (diff >= 86400) score = db[i].frequency * 5;
            else if (diff >= 3600) score = db[i].frequency * 20;
            else score = db[i].frequency * 40;
            
            if (bestScore < score)
            {
                bestScore = score;
                bestPath = db[i].path;
            }
            else if (bestScore == score)
            {
                if (bestPath == NULL || strcmp(db[i].path, bestPath) < 0) bestPath = db[i].path;
            }
        }
    }
    return bestPath;
}

void recordJump(const char* absolute_path)
{
    char dpPath[4096];
    snprintf(dpPath, sizeof(dpPath), "%s/.hop_frecency", getenv("HOME"));

    dbSize = 0;

    // --- STEP 1: READ EXISTING DATA ---
    FILE* file = fopen(dpPath, "r");
    if (file != NULL)
    {
        char line[5000];
        while (fgets(line, sizeof(line), file))
        {
            if (sscanf(line, "%[^,],%d,%ld", db[dbSize].path, &db[dbSize].frequency, &db[dbSize].recency) == 3) dbSize++;
        }
        fclose(file);
    }

    int found = 0;
    for (int i = 0; i < dbSize; i++) 
    {
        if (strcmp(db[i].path, absolute_path) == 0) 
        {
            db[i].frequency += 1;
            db[i].recency = time(NULL);
            found = 1;
            break;
        }
    }

    // --- STEP 3: ADD NEW RECORD LOGIC ---
    if (!found)
    {
        strcpy(db[dbSize].path, absolute_path);
        db[dbSize].frequency = 1;
        db[dbSize].recency = time(NULL);
        dbSize++;
    }

    // --- STEP 4: WRITE DATA BACK TO FILE ---
    FILE* out = fopen(dpPath, "w");
    if (out == NULL)
    {
        perror("Failed to open frecency database for writing");
        return;
    }
    
    for (int i = 0; i < dbSize; i++)
    {
        fprintf(out, "%s,%d,%ld\n", db[i].path, db[i].frequency, (long)db[i].recency);
    }
    
    fclose(out);
}