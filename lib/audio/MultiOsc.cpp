#include "MultiOsc.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

void MultiOsc::Init(float sampleRate) {
    this->sampleRate = sampleRate;
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc.SetAmp(1);
}

void MultiOsc::SetPulsewidth(float pulsewidth) {
    // pulsewidth is betw 0 and 1; threshold betw -1 and 1
    if (pulsewidth > 0 && pulsewidth < 1) {
        threshold = Utility::LimitFloat(pulsewidth * 2 - 1, -0.98, 0.98);
    }
}

float MultiOsc::Process() {
    lastSaw = sawSignal;
    sawSignal = osc.Process();
    if (lastSaw < threshold && sawSignal >= threshold) {
        // pulseSignal = 1;
    } else if (lastSaw >= 0 && sawSignal < 0) {
        // pulseSignal = -1;
        subSignal *= -1;
    }
    pulseSignal = sawSignal < threshold ? -1 : 1;
    saw2Signal = sawSignal <= 0 ? sawSignal * 2 + 1 : sawSignal * 2 - 1;
    return sawSignal;
}

void MultiOsc::Reset() {
    osc.Reset();
}
