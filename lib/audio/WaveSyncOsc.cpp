#include "WaveSyncOsc.h"

using namespace daisy;
using namespace daisysp;

void WaveSyncOsc::Init(float sampleRate, const char *sdPath) {
    this->sampleRate = sampleRate;
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc.SetAmp(1);

    wavPlayer.Init(sdPath);
}

float WaveSyncOsc::Process() {
    lastSaw = sawSignal;
    sawSignal = osc.Process();
    if (lastSaw < 0 && sawSignal >= 0) {
        currentSample = startSample;
    // } else if (lastSaw >= 0 && sawSignal < 0) {
    }

    float signal = s162f(wavPlayer.Stream());
    return signal;
}

void WaveSyncOsc::Reset() {
    currentSample = startSample;
    osc.Reset();
    wavPlayer.Restart();
}
