#ifndef ARENA_H
#define ARENA_H

#include "base.h"  // u8, u64, ALIGN_UP_POW2

#define ARENA_ALIGN ((u64)16)

typedef struct {
    u8 *Base;
    u64 Capacity;
    u64 Used;
} Arena;

Arena *Arena_Create(u64 capacity);
void   Arena_Destroy(Arena *a);
void   Arena_Reset(Arena *a);
void  *Arena_Push(Arena *a, u64 size);
void  *Arena_PushAligned(Arena *a, u64 size, u64 alignment);
void   Arena_Log(const Arena *a);

#endif
