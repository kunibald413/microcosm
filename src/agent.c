#include "base.h"  // u32, u64
#include "agent.h"

void Agent_Init(Agent *a, double x, double y)
{
    a->X = x;
    a->Y = y;
    a->Angle = 0.0;
    a->Radius = 15.0;
    a->r = 0.3;
    a->g = 0.8;
    a->b = 0.3;
    a->SpikeLength = 0.6;
    a->Health = 1.0;
    a->Age = 0;
    
    for (int i = 0; i < NUM_SENSORS; i++) {
        a->Sensors[i] = 0.0;
    }
}

void Agent_Update(Agent *a, double dt)
{
    a->Angle += dt;
}