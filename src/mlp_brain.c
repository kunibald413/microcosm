#include "mlp_brain.h"
#include "helpers.h"
#include <math.h>


static void MLPBox_Init(MLPBox *box, Pcg32 *rng)
{
    for (int i = 0; i < CONNS; i++) {
        box->W[i] = randf(rng, -3.0f, 3.0f);
        if (randf(rng, 0.0f, 1.0f) < 0.5f) {
            box->W[i] = 0.0f;
        }

        box->Id[i] = randi_range(rng, 0, BRAINSIZE);
        if (randf(rng, 0.0f, 1.0f) < 0.2f) {
            box->Id[i] = randi_range(rng, 0, INPUTSIZE);
        }

        box->Type[i] = 0;
        if (randf(rng, 0.0f, 1.0f) < 0.05f) {
            box->Type[i] = 1;
        }
    }
    box->Kp     = randf(rng, 0.9f, 1.1f);
    box->Gw     = randf(rng, 0.0f, 5.0f);
    box->Bias   = randf(rng, -2.0f, 2.0f);
    box->Out    = 0.0f;
    box->OldOut = 0.0f;
    box->Target = 0.0f;
}

void MLPBrain_Init(MLPBrain *b, Pcg32 *rng)
{
    for (int i = 0; i < BRAINSIZE; i++) {
        MLPBox_Init(&b->Boxes[i], rng);
    }
}

void MLPBrain_Copy(MLPBrain *dst, const MLPBrain *src)
{
    *dst = *src;
}

void MLPBrain_Tick(MLPBrain *b, const f64 *in, f64 *out)
{
    for (int i = 0; i < INPUTSIZE; i++) {
        b->Boxes[i].Out = (f32)in[i];
    }

    for (int i = INPUTSIZE; i < BRAINSIZE; i++) {
        MLPBox *box = &b->Boxes[i];
        f32 acc = 0.0f;
        for (int j = 0; j < CONNS; j++) {
            i32 idx  = box->Id[j];
            i32 type = box->Type[j];
            f32 val  = b->Boxes[idx].Out;
            if (type == 1) {
                val -= b->Boxes[idx].OldOut;
                val *= 10.0f;
            }
            acc += val * box->W[j];
        }
        acc *= box->Gw;
        acc += box->Bias;
        acc  = 1.0f / (1.0f + expf(-acc));
        box->Target = acc;
    }

    for (int i = 0; i < BRAINSIZE; i++) {
        b->Boxes[i].OldOut = b->Boxes[i].Out;
    }

    for (int i = INPUTSIZE; i < BRAINSIZE; i++) {
        MLPBox *box = &b->Boxes[i];
        box->Out += (box->Target - box->Out) * box->Kp;
    }

    for (int i = 0; i < OUTPUTSIZE; i++) {
        out[i] = (f64)b->Boxes[BRAINSIZE - 1 - i].Out;
    }
}

void MLPBrain_Mutate(MLPBrain *b, Pcg32 *rng, f32 mr, f32 mr2)
{
    for (int j = 0; j < BRAINSIZE; j++) {
        MLPBox *box = &b->Boxes[j];

        if (randf(rng, 0.0f, 1.0f) < mr) {
            box->Bias += randn(rng, 0.0f, mr2);
        }
        if (randf(rng, 0.0f, 1.0f) < mr) {
            box->Kp += randn(rng, 0.0f, mr2);
            if (box->Kp < 0.01f) box->Kp = 0.01f;
            if (box->Kp > 1.0f)  box->Kp = 1.0f;
        }
        if (randf(rng, 0.0f, 1.0f) < mr) {
            box->Gw += randn(rng, 0.0f, mr2);
            if (box->Gw < 0.0f) box->Gw = 0.0f;
        }
        if (randf(rng, 0.0f, 1.0f) < mr) {
            i32 rc = randi_range(rng, 0, CONNS);
            box->W[rc] += randn(rng, 0.0f, mr2);
        }
        if (randf(rng, 0.0f, 1.0f) < mr) {
            i32 rc = randi_range(rng, 0, CONNS);
            box->Type[rc] = 1 - box->Type[rc];
        }
        if (randf(rng, 0.0f, 1.0f) < mr) {
            i32 rc = randi_range(rng, 0, CONNS);
            box->Id[rc] = randi_range(rng, 0, BRAINSIZE);
        }
    }
}

void MLPBrain_Crossover(MLPBrain *dst,
                        const MLPBrain *a,
                        const MLPBrain *b,
                        Pcg32 *rng)
{
    for (int i = 0; i < BRAINSIZE; i++) {
        const MLPBox *src = (randf(rng, 0.0f, 1.0f) < 0.5f)
                          ? &a->Boxes[i]
                          : &b->Boxes[i];
        dst->Boxes[i] = *src;
    }
}