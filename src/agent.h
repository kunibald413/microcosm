#ifndef AGENT_H
#define AGENT_H

typedef struct {
    double X;
    double Y;

    double Angle; // heading, radians, 0 points right
    double Radius; // body radius, world units

    double r;
    double g;
    double b;

    double SpikeLength; // spike extension, 0 to 1
} Agent;

void Agent_Init(Agent *a, double x, double y);
void Agent_Update(Agent *a, double dt);

#endif