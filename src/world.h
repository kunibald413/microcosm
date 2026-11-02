#ifndef WORLD_H
#define WORLD_H

#include "base.h"  // u32, u64
#include "agent.h"
#include "pcg.h"
#include <stdbool.h>

enum {
    AGENT_NULL = 0,   // reserved index, always a zeroed agent
    MAX_AGENTS = 64,  // total slots, includes null slot at [0]
    MAX_FOOD_COLS = 128,
    MAX_FOOD_ROWS = 128,
    FOOD_ADD_PERIOD = 60,
    BASELOSS_PER_SEC = 1
};

static const double FOOD_CELL_SIZE = 16.0;
static const double FOOD_MAX = 1.0;
static const double HEALTH_LOSS_PER_SEC = 0.002;
static const double HEALTH_EAT_PER_SEC = 1.0;
static const double HEALTH_MAX = 2.0;

static const double EYE_OFFSET      = 0.4;    // radians, right of heading
static const double EYE_REACH       = 30.0;   // distance from body to eye
static const double EYE_SIGMA       = 50.0;   // sensor falloff width
static const double STEER_GAIN      = 1.5;    // turn response to signal
static const double SENSOR_BASELINE = 0.10;   // turn rate when no food

typedef struct {
    Agent  Agents[MAX_AGENTS];
    bool   Active[MAX_AGENTS];
    int    Count;
    int    DeathCount;
    double Width;
    double Height;
    Pcg32  Rng;

    float  Food[MAX_FOOD_ROWS][MAX_FOOD_COLS];
    int    FoodCols;
    int    FoodRows;
    int    Ticks;
} World;

void World_Init(World *w, int count, double width, double height);
void World_Update(World *w, double dt);

#endif