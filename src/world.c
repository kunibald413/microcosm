#include "world.h"
#include "math_const.h"
#include <math.h>  // cos, sin

// world units per second
static const double AGENT_SPEED = 30.0; 
// radians per second
static const double AGENT_WANDER_RATE = 0.6;

void World_Init(World *w, int count, double width, double height)
{
    w->Count  = count;
    w->Width  = width;
    w->Height = height;

    for (int i = 0; i < MAX_AGENTS; i++) {
        w->Active[i] = false;
    }

    w->Agents[AGENT_NULL] = (Agent){0};

    const uint64_t rng_seed = 1337;
    Pcg32_Srand(&w->Rng, rng_seed, PCG_DEFAULT_INC_64);

    for (int i = 1; i <= count; i++) {
        const double x = Pcg32_Unit(&w->Rng) * width;
        const double y = Pcg32_Unit(&w->Rng) * height;

        Agent_Init(&w->Agents[i], x, y);
        w->Agents[i].Angle = Pcg32_Unit(&w->Rng) * MATH_TAU;
        w->Active[i] = true;
    }

    for (int y = 0; y < FOOD_ROWS; y++) {
        for (int x = 0; x < FOOD_COLS; x++) {
            w->Food[y][x] = 0.0f;
        }
    }
    w->Ticks = 0;
}

void World_Update(World *w, double dt)
{
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        a->Angle += (Pcg32_Unit(&w->Rng) - 0.5)
                    * AGENT_WANDER_RATE * dt;

        // keep heading in [-PI, PI] so it stays small forever
        if (a->Angle < -MATH_PI) {
            a->Angle += MATH_TAU;
        }
        if (a->Angle >  MATH_PI) {
            a->Angle -= MATH_TAU;
        }

        a->X += cos(a->Angle) * AGENT_SPEED * dt;
        a->Y += sin(a->Angle) * AGENT_SPEED * dt;

        // world wraps as a torus: an agent that leaves one edge
        // enters at the opposite edge with the same heading
        if (a->X < 0.0)        a->X += w->Width;
        if (a->X >= w->Width)  a->X -= w->Width;
        if (a->Y < 0.0)        a->Y += w->Height;
        if (a->Y >= w->Height) a->Y -= w->Height;
    }

    w->Ticks++;

    // spawn food
    if (w->Ticks % FOOD_ADD_PERIOD == 0) {
        const int fx =
            (int)Pcg32_BoundedRand(&w->Rng, FOOD_COLS);
        const int fy =
            (int)Pcg32_BoundedRand(&w->Rng, FOOD_ROWS);
        w->Food[fy][fx] = (float)FOOD_MAX;
    }

    // agents consume food
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }
        const Agent *a = &w->Agents[i];

        const int cx = (int)(a->X / FOOD_CELL);
        const int cy = (int)(a->Y / FOOD_CELL);
        if (cx < 0 || cx >= FOOD_COLS) {
            continue;
        }
        if (cy < 0 || cy >= FOOD_ROWS) {
            continue;
        }

        if (w->Food[cy][cx] > 0.0f) {
            w->Food[cy][cx] -= 0.02f;
            if (w->Food[cy][cx] < 0.0f) {
                w->Food[cy][cx] = 0.0f;
            }
        }
    }
}