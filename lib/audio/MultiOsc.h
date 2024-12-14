#ifndef MULTIOSC_H
#define MULTIOSC_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class MultiOsc {

    public:
        void Init(float sampleRate);
        float Process();
        void Reset();
        void SetFreq(float frequency) { osc.SetFreq(frequency); }
        void SetPulsewidth(float pulsewidth);

        float Saw() { return sawSignal; }
        float Pulse() { return pulseSignal; }
        float Sub() { return subSignal; }
        float Saw2() { return saw2Signal; }

    private:
        float sampleRate = 48000;
        Oscillator osc;
        float threshold = 0;

        float sawSignal = 1;
        float pulseSignal = 1;
        float subSignal = 1;
        float saw2Signal = 1;
        float lastSaw = 1;
};





#endif
