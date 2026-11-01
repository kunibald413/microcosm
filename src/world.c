#include "base.h"  // u32, u64
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

    w->FoodCols = (int)(width  / FOOD_CELL_SIZE);
    w->FoodRows = (int)(height / FOOD_CELL_SIZE);
    if (w->FoodCols > MAX_FOOD_COLS) {
        w->FoodCols = MAX_FOOD_COLS;
    }
    if (w->FoodRows > MAX_FOOD_ROWS) {
        w->FoodRows = MAX_FOOD_ROWS;
    }
    if (w->FoodCols < 1) {
        w->FoodCols = 1;
    }
    if (w->FoodRows < 1) {
        w->FoodRows = 1;
    }
    w->Ticks = 0;

    for (int y = 0; y < w->FoodRows; y++) {
        for (int x = 0; x < w->FoodCols; x++) {
            w->Food[y][x] = 0.0f;
        }
    }

    for (int i = 0; i < MAX_AGENTS; i++) {
        w->Active[i] = false;
    }

    w->Agents[AGENT_NULL] = (Agent){0};

    const u64 rng_seed = 1337;
    Pcg32_Srand(&w->Rng, rng_seed, PCG_DEFAULT_INC_64);

    for (int i = 1; i <= count; i++) {
        const double x = Pcg32_Unit(&w->Rng) * width;
        const double y = Pcg32_Unit(&w->Rng) * height;

        Agent_Init(&w->Agents[i], x, y);
        w->Agents[i].Angle = Pcg32_Unit(&w->Rng) * MATH_TAU;
        w->Active[i] = true;
    }
}


static double SenseFood(World *w, const Agent *a, double eye_offset)
{
    const double eye_ang = a->Angle + eye_offset;
    const double ex = a->X + cos(eye_ang) * EYE_REACH;
    const double ey = a->Y + sin(eye_ang) * EYE_REACH;

    const double two_sigma2 = 2.0 * EYE_SIGMA * EYE_SIGMA;
    const double reach = 2.0 * EYE_SIGMA;

    const int x0 = (int)((ex - reach) / FOOD_CELL_SIZE);
    const int x1 = (int)((ex + reach) / FOOD_CELL_SIZE) + 1;
    const int y0 = (int)((ey - reach) / FOOD_CELL_SIZE);
    const int y1 = (int)((ey + reach) / FOOD_CELL_SIZE) + 1;

    double sum = 0.0;
    for (int y = y0; y <= y1; y++) {
        if (y < 0 || y >= w->FoodRows) continue;
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || x >= w->FoodCols) continue;
            if (w->Food[y][x] <= 0.0f) continue;

            const double fx = ((double)x + 0.5) * FOOD_CELL_SIZE;
            const double fy = ((double)y + 0.5) * FOOD_CELL_SIZE;
            const double dx = fx - ex;
            const double dy = fy - ey;
            const double d2 = dx * dx + dy * dy;

            const double amount = (double)w->Food[y][x] / FOOD_MAX;
            sum += amount * exp(-d2 / two_sigma2);
        }
    }
    return sum;
}

void World_Update(World *w, double dt)
{
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        a->Sensors[SENSOR_LEFT]  =
            SenseFood(w, a, a->Angle - EYE_OFFSET);
        a->Sensors[SENSOR_RIGHT] =
            SenseFood(w, a, a->Angle + EYE_OFFSET);

        a->Angle += (Pcg32_Unit(&w->Rng) - 0.5)
                    * AGENT_WANDER_RATE * dt;

        a->Angle += STEER_GAIN
                    * (a->Sensors[SENSOR_RIGHT]
                       - a->Sensors[SENSOR_LEFT]) * dt;

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

    if (w->Ticks % FOOD_ADD_PERIOD == 0) {
        const int fx = (int)Pcg32_BoundedRand(
            &w->Rng, (u32)w->FoodCols);
        const int fy = (int)Pcg32_BoundedRand(
            &w->Rng, (u32)w->FoodRows);
        w->Food[fy][fx] = (float)FOOD_MAX;
    }

    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }
        Agent *a = &w->Agents[i];

        const int cx = (int)(a->X / FOOD_CELL_SIZE);
        const int cy = (int)(a->Y / FOOD_CELL_SIZE);
        if (cx < 0 || cx >= w->FoodCols) {
            continue;
        }
        if (cy < 0 || cy >= w->FoodRows) {
            continue;
        }

        if (w->Food[cy][cx] > 0.0f) {
            const double eaten = 0.5 * dt;
            w->Food[cy][cx] -= (float)eaten;
            if (w->Food[cy][cx] < 0.0f) {
                w->Food[cy][cx] = 0.0f;
            }

            a->Health += HEALTH_EAT_PER_SEC * dt;
            if (a->Health > HEALTH_MAX) {
                a->Health = HEALTH_MAX;
            }
        }
    }

    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        a->Health -= HEALTH_LOSS_PER_SEC * dt;
        a->Age += 1;

        if (a->Health <= 0.0) {
            w->Active[i] = false;
            w->Count--;
            w->DeathCount++;
        }
    }
}