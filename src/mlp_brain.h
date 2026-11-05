#ifndef MLP_BRAIN_H
#define MLP_BRAIN_H

#include "base.h"
#include "pcg.h"
#include "config.h"

typedef struct {
    f32 W[CONNS];
    i32 Id[CONNS];
    i32 Type[CONNS];
    f32 Kp;
    f32 Gw;
    f32 Bias;
    f32 Target;
    f32 Out;
    f32 OldOut;
} MLPBox;

typedef struct {
    MLPBox Boxes[BRAINSIZE];
} MLPBrain;

void MLPBrain_Init(MLPBrain *b, Pcg32 *rng);
void MLPBrain_Copy(MLPBrain *dst, const MLPBrain *src);
void MLPBrain_Tick(MLPBrain *b, const f64 *in, f64 *out);
void MLPBrain_Mutate(MLPBrain *b, Pcg32 *rng, f32 mr, f32 mr2);
void MLPBrain_Crossover(MLPBrain *dst, const MLPBrain *a, const MLPBrain *b, Pcg32 *rng);

#endif