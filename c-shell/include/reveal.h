#ifndef REVEAL_H
#define REVEAL_H

#include "intrinsics.h"

void revealRecursive(const char* base_path, const char* prefix, int aFlag);
void executeReveal(Command* command);

#endif