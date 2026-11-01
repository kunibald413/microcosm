
#ifndef PCG_H
#define PCG_H

#include <stdint.h>

#define PCG_DEFAULT_INC_64 1442695040888963407ULL

// Source from https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.h
typedef struct {
    uint64_t State;
    uint64_t Inc;
} Pcg32;

// Returns a uniform value in [0, 2^32)
uint32_t Pcg32_Random(Pcg32 *rng);

// Sets the seed and stream. Call one time before first use.
void Pcg32_Srand(Pcg32 *rng, uint64_t initstate, uint64_t initseq);

// Returns a uniform value in [0, bound). bound must be >= 1.
uint32_t Pcg32_BoundedRand(Pcg32 *rng, uint32_t bound);

static inline double Pcg32_Unit(Pcg32 *rng)
{
    return (double)Pcg32_Random(rng) / 4294967296.0; // 2^32
}

#endif