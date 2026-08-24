#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <time.h>
#include "parser.h"

typedef struct {
    char path[4096];
    int frequency;
    time_t recency;
} FrecencyRecord;

void recordJump(const char* absolute_path);

void executeHop(Command* command);
void executeReveal(Command* command);
void executePeek(Command* command);
void executeLocate(Command* command);

#endif