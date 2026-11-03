#include "base.h"  // u32, u64
#include "world.h"
#include "math_const.h"
#include <math.h>  // cos, sin

// world units per second
static const double AGENT_SPEED = 20.0; 
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


static inline f64 CAP(double a) 
{
	if (a<0.0) return 0.0;
	if (a>1.0) return 1.0;
	return a;
}

double get_angle(double x, double y) {
    if (x == 0.0 && y == 0.0) return 0.0;
    return atan2(y, x);
}

void HandleInputs(World *w)
{
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        // sensors
        a->Sensors[SENSOR_HEALTH] = CAP(a->Health / HEALTH_MAX);
        
        int cx = (int)(a->X / FOOD_CELL_SIZE);
        int cy = (int)(a->Y / FOOD_CELL_SIZE);
        a->Sensors[SENSOR_FOOD] = w->Food[cx][cy] / FOOD_MAX;

        // closer agents smell stronger 
        // (linear falloff: 1.0 at touch, 0.0 at edge of range)
        double smell_accum = 0.0;

        // nearby, fast-moving agents are louder (proximity * speed)
        double sound_accum = 0.0;
        
        // listen to other agents
        double hearing_accum = 0.0;

        for (int j = 0; j < MAX_AGENTS; j++) {
            if (!w->Active[j]) continue;
            if (j == i) continue; // skip self

            Agent *other = &w->Agents[j];

            if (a->X < other->X - SENSOR_RANGE 
                || a->X > other->X + SENSOR_RANGE
                || a->Y < other->Y - SENSOR_RANGE
                || a->Y > other->Y + SENSOR_RANGE) 
            {
                continue;
            }

            double dx = a->X - other->X;
            double dy = a->Y - other->Y;
            double distance = sqrt(dx * dx + dy * dy);

            if (distance >= SENSOR_RANGE) continue;
            smell_accum += (SENSOR_RANGE - distance) / SENSOR_RANGE;
            
            // how loud this agent is
            double speed = fmax(fabs(other->WheelSpeed1), fabs(other->WheelSpeed2)); 
            sound_accum += (SENSOR_RANGE - distance) / SENSOR_RANGE * speed;

            hearing_accum += other->SoundVolume * (SENSOR_RANGE -  distance) / SENSOR_RANGE;

            dx = other->X - a->X;
            dy = other->Y - a->Y;
            double angle = get_angle(dx, dy);   // direction from a toward other, in [-pi, pi]

            (void)angle;
        }
    }
}

void ProcessBrains(World *w)
{
    (void)w;
}

void ProcessOutputs(World *w, double dt) 
{
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }

        Agent *a = &w->Agents[i];

        // move
        a->Angle += (Pcg32_Unit(&w->Rng) - 0.5)
                    * AGENT_WANDER_RATE * dt;

        // keep heading in [-PI, PI] so it stays small forever
        if (a->Angle < -MATH_PI) a->Angle += MATH_TAU;
        if (a->Angle >  MATH_PI) a->Angle -= MATH_TAU;

        a->X += cos(a->Angle) * AGENT_SPEED * dt;
        a->Y += sin(a->Angle) * AGENT_SPEED * dt;

        // world wraps as a torus: an agent that leaves one edge
        // enters at the opposite edge with the same heading
        if (a->X < 0.0)        a->X += w->Width;
        if (a->X >= w->Width)  a->X -= w->Width;
        if (a->Y < 0.0)        a->Y += w->Height;
        if (a->Y >= w->Height) a->Y -= w->Height;
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
}

void World_Update(World *w, double dt)
{
    w->Ticks++;

    // spawn food
    if (w->Ticks % FOOD_ADD_PERIOD == 0) {
        const int fx = (int)Pcg32_BoundedRand(
            &w->Rng, (u32)w->FoodCols);
        const int fy = (int)Pcg32_BoundedRand(
            &w->Rng, (u32)w->FoodRows);
        w->Food[fy][fx] = (float)FOOD_MAX;
    }

    HandleInputs(w);

    ProcessBrains(w);

    ProcessOutputs(w, dt);

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