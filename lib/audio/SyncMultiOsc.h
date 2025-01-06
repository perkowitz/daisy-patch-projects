#ifndef SYNCMULTIOSC_H
#define SYNCMULTIOSC_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class SyncMultiOsc {

    public:
        void Init(float sampleRate);
        float Process();
        void Reset();
        void SetFreq(float frequency);
        void SetSyncFactor(float syncFactor);
        void SetPulsewidth(float pulsewidth);

        float Saw() { return sawSignal; }
        float Pulse() { return pulseSignal; }
        float Sub() { return subSignal; }
        float Saw2() { return saw2Signal; }

    private:
        float sampleRate = 48000;
        Oscillator timbreOsc;
        Oscillator pitchOsc;
        float threshold = 0;

        float syncFactor = 1;
        float frequency = 440;

        float lastTimbreSaw = 1;
        float pitchSignal = 1;
        float lastPitchSaw = 1;

        float sawSignal = 1;
        float pulseSignal = 1;
        float subSignal = 1;
        float saw2Signal = 1;

};





#endif
