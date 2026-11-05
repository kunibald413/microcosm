#include "base.h"  // u32, u64
#include "agent.h"

void Agent_Init(Agent *a, double x, double y)
{
    a->X = x;
    a->Y = y;
    a->Angle = 0.0;
    a->Radius = 15.0;
    a->Red = 0.3;
    a->Green = 0.8;
    a->Blue = 0.3;
    a->WheelSpeed1 = 0.0;
    a->WheelSpeed2 = 0.0;
    a->SoundVolume = 1.0;
    a->SpikeLength = 0.6;
    a->Health = 1.0;
    a->Age = 0;
    
    for (int i = 0; i < NUM_SENSORS; i++) {
        a->Sensors[i] = 0.0;
    }

    // Eyes: spread 4 cones roughly around the body
    // each ~45° half-FOV.
    for (int q = 0; q < NUM_EYES; q++) {
        a->EyeDir[q] = (2.0 * M_PI * q / NUM_EYES) - M_PI;  // spread evenly, wrapped
        a->EyeFov[q] = M_PI / 4.0;                          // +/- 45°
    }

    a->EyeSensMod   = 0.5;
    a->SmellSensMod = 0.5;
    a->SoundSensMod = 0.5;
    a->HearSensMod  = 0.5;
    a->BloodSensMod = 0.5;
}

