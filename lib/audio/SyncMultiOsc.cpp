#include "SyncMultiOsc.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

void SyncMultiOsc::Init(float sampleRate) {
    this->sampleRate = sampleRate;
    pitchOsc.Init(sampleRate);
    pitchOsc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    pitchOsc.SetAmp(1);
    timbreOsc.Init(sampleRate);
    // timbreOsc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    timbreOsc.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);
    timbreOsc.SetAmp(1);
}

void SyncMultiOsc::SetPulsewidth(float pulsewidth) {
    // pulsewidth is betw 0 and 1; threshold betw -1 and 1
    if (pulsewidth > 0 && pulsewidth < 1) {
        threshold = Utility::LimitFloat(pulsewidth * 2 - 1, -0.98, 0.98);
    }
}

void SyncMultiOsc::SetFreq(float frequency) { 
    this->frequency = frequency;
    pitchOsc.SetFreq(frequency); 
    timbreOsc.SetFreq(frequency * syncFactor);
}
void SyncMultiOsc::SetSyncFactor(float syncFactor) { 
    if (syncFactor >= 1) {
        this->syncFactor = syncFactor;
        timbreOsc.SetFreq(frequency * syncFactor); 
    }
}

float SyncMultiOsc::Process() {

    // check the pitch osc for zero crossings
    lastPitchSaw = pitchSignal;
    pitchSignal = pitchOsc.Process();
    // bool pitchZeroCrossing = false;
    if (lastPitchSaw < 0 && pitchSignal >= 0) {
        // pitchZeroCrossing = true;
        timbreOsc.Reset();
    }

    // check the timbre osc for zero crossings
    lastTimbreSaw = sawSignal;
    sawSignal = timbreOsc.Process();
    if (lastTimbreSaw < threshold && sawSignal >= threshold) {
        // pulseSignal = 1;
    } else if (lastTimbreSaw >= 0 && sawSignal < 0) {
        // pulseSignal = -1;
        subSignal *= -1;
    }
    pulseSignal = sawSignal < threshold ? -1 : 1;
    saw2Signal = sawSignal <= 0 ? sawSignal * 2 + 1 : sawSignal * 2 - 1;
    return sawSignal;
}

void SyncMultiOsc::Reset() {
    timbreOsc.Reset();
}
