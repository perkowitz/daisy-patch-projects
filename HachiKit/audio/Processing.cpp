#include "Processing.h"

using namespace daisy;
using namespace daisysp;
using namespace Processing;

/***** Gate ***************/

float GateProcess(Gate *gate, float signal) {
    if (signal > 0) {
        return signal < gate->threshold ? 0 : signal;
    }
    return signal > -1 * gate->threshold ? 0 : signal;
}

void Gate_Process(Gate *gate, float &signal) {
    if (signal > 0) {
        signal = signal < gate->threshold ? 0 : signal;
    }
    signal = signal > -1 * gate->threshold ? 0 : signal;
}


/***** SoftGate ***************/

float SoftGateProcess(SoftGate *softGate, float signal) {
    if (signal > 0) {
        return signal >= softGate->threshold ? 
            signal : 
            std::max(0.0f, signal * softGate->slope - softGate->threshold);
    }
    return signal <= softGate->threshold ? 
        signal : 
        std::min(0.0f, signal * softGate->slope + softGate->threshold);
}

void SoftGate_Process(SoftGate *softGate, float &signal) {
    if (signal > 0) {
        signal = 
            signal >= softGate->threshold ? 
            signal : 
            std::max(0.0f, signal * softGate->slope - softGate->threshold);
    }
    signal = 
        signal <= softGate->threshold ? 
        signal : 
        std::min(0.0f, signal * softGate->slope + softGate->threshold);
}


/***** Limiter ***************/

float HardLimiterProcess(Gate *gate, float signal) {
    return 0.0f;
}

void HardLimiter_Process(Gate *gate, float &signal) {
}
