#include "world.h"
#include "math_const.h"
#include <math.h>  // cos, sin
#include <stdlib.h> // rand, RAND_MAX

// world units per second
static const double AGENT_SPEED = 60.0; 
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

    for (int i = 1; i <= count; i++) {
        const double x = ((double)rand() / RAND_MAX) * width;
        const double y = ((double)rand() / RAND_MAX) * height;

        Agent_Init(&w->Agents[i], x, y);
        w->Agents[i].Angle = ((double)rand() / RAND_MAX) * MATH_TAU;
        w->Active[i] = true;
    }
}

void World_Update(World *w, double dt)
{
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        a->Angle += (((double)rand() / RAND_MAX) - 0.5)
                    * AGENT_WANDER_RATE * dt;

        a->X += cos(a->Angle) * AGENT_SPEED * dt;
        a->Y += sin(a->Angle) * AGENT_SPEED * dt;

        // world wraps as a torus: an agent that leaves one edge
        // enters at the opposite edge with the same heading
        if (a->X < 0.0)        a->X += w->Width;
        if (a->X >= w->Width)  a->X -= w->Width;
        if (a->Y < 0.0)        a->Y += w->Height;
        if (a->Y >= w->Height) a->Y -= w->Height;
    }
}