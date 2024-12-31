#ifndef WAVESYNCOSC_H
#define WAVESYNCOSC_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class WaveSyncOsc {

    public:
        void Init(float sampleRate, const char *sdPath);
        float Process();
        void Reset();
        void SetFreq(float frequency) { osc.SetFreq(frequency); }
        void SetFile(u8 index) { wavPlayer.Open(index); }

        float Saw() { return sawSignal; }
        float Wave() { return signal; }

    private:
        float sampleRate = 48000;
        WavPlayer wavPlayer;
        u32 currentSample = 0;
        u32 startSample = 0;
        Oscillator osc;
        float lastSaw = 0;
        float sawSignal = 0;
        float signal = 0;
};





#endif
