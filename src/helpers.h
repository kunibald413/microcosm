#ifndef HELPERS_H
#define HELPERS_H

#include "base.h"
#include "pcg.h"
#include <math.h>


f32 randf(Pcg32 *rng, f32 lo, f32 hi);
i32 randi_range(Pcg32 *rng, i32 lo, i32 hi);
f32 randn(Pcg32 *rng, f32 mean, f32 stddev);

#endif