#ifndef PROCESSING_H
#define PROCESSING_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../utility.h"

namespace Processing {

/**
 * Gate
 * 
 * Cuts off signal below a threshold.
 */
struct Gate {
    float threshold;
};
float GateProcess(Gate *gate, float signal);
void Gate_Process(Gate *gate, float &signal);
// void Gate_Process(Gate *gate, float &left, float &right);

// Gate _MAIN_GATE;
// Gate *mainGate = &_MAIN_GATE;



/**
 * SoftGate
 * 
 * Reduces signal at a slope below a threshold.
 */
struct SoftGate {
    float threshold;
    float slope;
};
float SoftGateProcess(SoftGate *softGate, float signal);
void SoftGate_Process(SoftGate *softGate, float &signal);


/**
 * HardLimiter
 * 
 * Cuts off a signal at a limit.
 */
struct HardLimiter {
    float limit;
    float makeupGain;
};
float HardLimiterProcess(HardLimiter *limiter, float signal);
void HardLimiter_Process(HardLimiter *limiter, float &signal);


/**
 * LinearCompressor
 * 
 * Compresses the signal above a threshold by a simple ratio.
 */
struct LinearCompressor {
    float threshold;
    float ratio;
    float makeupGain;
};

}   // namespace

#endif