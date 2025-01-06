#include "WaveSyncOsc.h"

using namespace daisy;
using namespace daisysp;

void WaveSyncOsc::Init(float sampleRate) {
    this->sampleRate = sampleRate;
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc.SetAmp(1);
}

float WaveSyncOsc::Process() {
    lastSaw = sawSignal;
    sawSignal = osc.Process();
    if (lastSaw < 0 && sawSignal >= 0) {
        currentSample = startSample;
    // } else if (lastSaw >= 0 && sawSignal < 0) {
    }

    signal = buffer[currentSample];
    currentSample = (currentSample + 1) % bufferSize;
    return signal;
}

void WaveSyncOsc::Reset() {
    currentSample = startSample;
    osc.Reset();
}
