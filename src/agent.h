#ifndef AGENT_H
#define AGENT_H

#include "base.h"  // u32, u64

#include "mlp_brain.h" // MLPBrain




enum {
    // Eye 0: proximity, red, green, blue
    SENSOR_EYE0_PROX = 0,
    SENSOR_EYE0_R,
    SENSOR_EYE0_G,
    SENSOR_EYE0_B,

    // Food in the agent's cell
    SENSOR_FOOD,

    // Eye 1: proximity, red, green, blue
    SENSOR_EYE1_PROX,
    SENSOR_EYE1_R,
    SENSOR_EYE1_G,
    SENSOR_EYE1_B,

    // Sound and smell from other agents
    SENSOR_SOUND,
    SENSOR_SMELL,

    // Own health
    SENSOR_HEALTH,

    // Eye 2: proximity, red, green, blue
    SENSOR_EYE2_PROX,
    SENSOR_EYE2_R,
    SENSOR_EYE2_G,
    SENSOR_EYE2_B,

    // Internal clocks
    SENSOR_CLOCK1,
    SENSOR_CLOCK2,

    // Hearing other agents
    SENSOR_HEARING,

    // Blood sensor
    SENSOR_BLOOD,

    // Temperature discomfort
    SENSOR_TEMP,

    // Eye 3: proximity, red, green, blue
    SENSOR_EYE3_PROX,
    SENSOR_EYE3_R,
    SENSOR_EYE3_G,
    SENSOR_EYE3_B,

    NUM_SENSORS,

    // Aliases: current world.c writes food sensing to these two slots
    SENSOR_LEFT  = SENSOR_EYE0_PROX,
    SENSOR_RIGHT = SENSOR_EYE1_PROX
};

enum {
    // Brain output slots
    OUTPUT_WHEEL1 = 0,   // left wheel speed
    OUTPUT_WHEEL2,       // right wheel speed
    OUTPUT_RED,          // body color red
    OUTPUT_GREEN,        // body color green
    OUTPUT_BLUE,         // body color blue
    OUTPUT_SPIKE,        // spike length target
    OUTPUT_BOOST,        // boost flag, > 0.5 means on
    OUTPUT_SOUND,        // sound multiplier
    OUTPUT_GIVE,         // food giving flag, > 0.5 means on

    NUM_OUTPUTS
};

enum {
    NUM_EYES = 4,
};


_Static_assert(INPUTSIZE  == NUM_SENSORS, "INPUTSIZE must match NUM_SENSORS");
_Static_assert(OUTPUTSIZE == NUM_OUTPUTS, "OUTPUTSIZE must match NUM_OUTPUTS");
_Static_assert(INPUTSIZE + OUTPUTSIZE <= BRAINSIZE, "network too small for inputs and outputs");

typedef struct {
    double X;
    double Y;

    double Angle; // heading, radians, 0 points right
    double Radius; // body radius, world units

    double Red;
    double Green;
    double Blue;

    double WheelSpeed1;
    double WheelSpeed2;

    double SoundVolume; // how loud agent is

    double SpikeLength; // spike extension, 0 to 1

    double Herbivore;   /* 0 = carnivore, 1 = herbivore */
    double Boost;       /* > 0.5 means boost is on */
    double Give;        /* > 0.5 means food giving is on */
    b8     Spiked;      /* was hit by a spike this tick */
    double DeltaFood;   /* food change, for drawing only */

    double Health;
    int    Age;

    double EyeSensMod;         // vision sensitivity gene, scales all eye inputs
    double SmellSensMod;       // smell sensitivity gene
    double SoundSensMod;       // ambient-sound sensitivity gene
    double HearSensMod;        // voice/hearing sensitivity gene
    double BloodSensMod;       // blood-sensor sensitivity gene

    double Sensors[NUM_SENSORS];
    double BrainOutputs[NUM_OUTPUTS];

    double EyeDir[NUM_EYES];   // direction of each eye, radians
    double EyeFov[NUM_EYES];   // field of view for each eye, radians

    MLPBrain Brain;
} Agent;

void Agent_Init(Agent *a, double x, double y, Pcg32 *rng);

#endif