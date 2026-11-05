#include "helpers.h"
#include <assert.h>

f32 randf(Pcg32 *rng, f32 lo, f32 hi)
{
    return lo + (f32)Pcg32_Unit(rng) * (hi - lo);
}

i32 randi_range(Pcg32 *rng, i32 lo, i32 hi)
{
    assert(hi > lo);
    return lo + (i32)Pcg32_BoundedRand(rng, (u32)(hi - lo));
}

f32 randn(Pcg32 *rng, f32 mean, f32 stddev)
{
    f32 u1 = (f32)Pcg32_Unit(rng);
    f32 u2 = (f32)Pcg32_Unit(rng);
    if (u1 < 1e-7f) u1 = 1e-7f;
    f32 z = sqrtf(-2.0f * logf(u1)) * cosf(6.2831853f * u2);
    return mean + z * stddev;
}