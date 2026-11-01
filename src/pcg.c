
#include "pcg.h"

uint32_t Pcg32_Random(Pcg32 *rng)
{
    const uint64_t oldState = rng->State;
    rng->State = oldState * 6364136223846793005ULL + (rng->Inc | 1);

    const uint32_t xorshifted =
        (uint32_t)(((oldState >> 18u) ^ oldState) >> 27u);
    const uint32_t rot = (uint32_t)(oldState >> 59u);

    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// Source from https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.c
void Pcg32_Srand(Pcg32 *rng, uint64_t initstate, uint64_t initseq)
{
    rng->State = 0U;
    rng->Inc = (initseq << 1u) | 1u;
    Pcg32_Random(rng);
    rng->State += initstate;
    Pcg32_Random(rng);
}

// Source from https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.c
uint32_t Pcg32_BoundedRand(Pcg32 *rng, uint32_t bound)
{
    // Reject values below the threshold to remove modulo bias.
    // Uses 32-bit ops: (2^32 - bound) % bound == 2^32 % bound.
    const uint32_t threshold = -bound % bound;

    for (;;) {
        const uint32_t r = Pcg32_Random(rng);
        if (r >= threshold) {
            return r % bound;
        }
    }
}