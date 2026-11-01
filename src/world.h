#ifndef WORLD_H
#define WORLD_H

#include "agent.h"
#include <stdbool.h>

enum {
    AGENT_NULL = 0,   // reserved index, always a zeroed agent
    MAX_AGENTS = 64   // total slots, includes null slot at [0]
};
typedef struct {
    Agent  Agents[MAX_AGENTS];
    bool   Active[MAX_AGENTS];
    int    Count;
    double Width;
    double Height;
} World;

void World_Init(World *w, int count, double width, double height);
void World_Update(World *w, double dt);

#endif