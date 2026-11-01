#include "arena.h"
#include <assert.h>
#include <stdlib.h>  // calloc, free, abort

Arena *Arena_Create(u64 capacity)
{
    Arena *a = calloc(1, sizeof(Arena) + (size_t)capacity);
    if (a == NULL) {
        fprintf(stderr, "Arena_Create: %llu bytes failed\n",
                (unsigned long long)capacity);
        abort();
    }
    a->Base     = (u8 *)(a + 1);
    a->Capacity = capacity;
    a->Used     = 0;
    return a;
}

void Arena_Destroy(Arena *a)
{
    free(a);
}

void Arena_Reset(Arena *a)
{
    a->Used = 0;
}

static u64 Arena_AlignPad(const Arena *a, u64 alignment)
{
    assert(alignment > 0 && (alignment & (alignment - 1)) == 0);

    const u64 addr = (u64)(a->Base + (size_t)a->Used);
    const u64 aligned = ALIGN_UP_POW2(addr, alignment);
    return aligned - addr;
}

void *Arena_PushAligned(Arena *a, u64 size, u64 alignment)
{
    const u64 pad   = Arena_AlignPad(a, alignment);
    const u64 total = size + pad;

    if (total > a->Capacity - a->Used) {
        fprintf(stderr,
                "Arena_Push: need %llu bytes, have %llu of %llu\n",
                (unsigned long long)total,
                (unsigned long long)(a->Capacity - a->Used),
                (unsigned long long)a->Capacity);
        abort();
    }

    void *p = a->Base + (size_t)a->Used + (size_t)pad;
    a->Used += total;
    return p;
}

void *Arena_Push(Arena *a, u64 size)
{
    return Arena_PushAligned(a, size, ARENA_ALIGN);
}

void Arena_Log(const Arena *a)
{
    fprintf(stderr, "arena: used=%llu cap=%llu (%.2f%%) ptr=%p\n",
            (unsigned long long)a->Used,
            (unsigned long long)a->Capacity,
            (double)a->Used / (double)a->Capacity * 100.0,
            (void *)(a->Base + (size_t)a->Used));
}
