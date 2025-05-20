#include "Tom8.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float Tom8::presets[][Tom8::PARAM_COUNT] = {
    // freq, freq1, decay, osc2 level, ringmod level, squeeze, reverb
    {80, 80, 1.100, 1.0, 0.5, 0.05, 0.05},
    {171, 171, 0.682, 0.5, 1, 0.05, 0.05, 0.05},
    {94, 94, 0.593, 1, 5, 0.05, 0.05, 0.05},
    {110, 110, 0.579, 0.9, 10, 0.05, 0.05, 0.05},
    {77, 77, 1.926, 0.3, 0.005, 0.05, 0.05, 0.05},
    {200, 200, 1.812, 0.7, 0, 0.05, 0.05, 0.05},
    {197, 197, 1.297, 0.1, 0, 0.05, 0.05, 0.05},
    {79, 161, 0.952, 0.9, 0.9, 0.1, 0.05, 0.8}
};

float Tom8::mtPresets[][Tom8::PARAM_COUNT] = {
    // freq, freq1, decay, osc2 level, ringmod level, squeeze, reverb
    {96, 112, 1.100, 0.1, 0.5, 0.05, 0.05, 0.05},
    {210, 214, 0.682, 1, 0, 0.05, 0.05, 0.05},
    {156, 299, 0.593, 0.6, 0.5, 0.05, 0.05, 0.05},
    {155, 155, 0.579, 0.0, 0.9, 0.05, 0.05, 0.05},
    {99, 119, 1.926, 0.3, 0.3, 0.05, 0.05, 0.05},
    {280, 280, 1.812, 0, 0, 0.05, 0.05, 0.05},
    {301, 288, 1.297, 0.9, 0.4, 0.05, 0.05, 0.05},
    {153, 225, 0.867, 0.9, 0.9, 0.1, 0.05, 0.8}
};

float Tom8::htPresets[][Tom8::PARAM_COUNT] = {
    // freq, freq1, decay, osc2 level, ringmod level, squeeze, reverb
    {131, 137, 1.100, 1.0, 0.2, 0.05, 0.05, 0.05},
    {255, 345, 0.682, 0.2, 0.6, 0.05, 0.05, 0.05},
    {199, 504, 0.593, 0.9, 0.9, 0.05, 0.05, 0.05},
    {199, 700, 0.579, 0.3, 0.3, 0.05, 0.05, 0.05},
    {111, 119, 1.926, 0.9, 0.2, 0.05, 0.05, 0.05},
    {400, 405, 1.812, 0.9, 0.1, 0.05, 0.05, 0.05},
    {483, 545, 1.297, 0.5, 0.9, 0.05, 0.05, 0.05},
    {222, 459, 0.536, 0.9, 0.9, 0.1, 0.05, 0.8}
};


void Tom8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 80);
}

void Tom8::Init(std::string slot, float sample_rate, float frequency) {

    this->slot = slot;

    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    oscFilter.Init(sample_rate);
    oscFilter.SetRes(0.5);
    SetParam(PARAM_FREQUENCY, frequency);  // sets oscillator and bpf frequency

    osc2.Init(sample_rate);
    osc2.SetWaveform(Oscillator::WAVE_SIN);
    SetParam(PARAM_FREQUENCY2, frequency);
    SetParam(PARAM_RINGMOD_LEVEL, 0.5);

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(5);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-10);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, 1.1);

    noise.Init();
    noiseEnv.Init(sample_rate);
    noiseEnv.SetMax(1);
    noiseEnv.SetMin(0);
    noiseEnv.SetCurve(-10);
    noiseEnv.SetTime(ADENV_SEG_ATTACK, 0.050);
    noiseFilter.Init();
    noiseFilter.SetFilterMode(OnePole::FILTER_MODE_LOW_PASS);
    noiseFilter.SetFrequency(0.005);

    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    pitchEnv.SetCurve(-20);
    pitchEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    pitchEnv.SetTime(ADENV_SEG_DECAY, 0.25);
}

float Tom8::Process() {

    float pitchEnvSignal = pitchEnv.Process();
    float ampEnvSignal = ampEnv.Process();

    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + 180 * pitchEnvSignal * parameters[PARAM_BEND].GetScaledValue());
    osc2.SetFreq(parameters[PARAM_FREQUENCY2].GetScaledValue() + 180 * pitchEnvSignal * parameters[PARAM_BEND].GetScaledValue());
    float oscSignal = osc.Process();
    float osc2Signal = osc2.Process();
    oscSignal += parameters[PARAM_OSC2_LEVEL].GetScaledValue() * osc2Signal;
    oscSignal += parameters[PARAM_RINGMOD_LEVEL].GetScaledValue() * oscSignal * osc2Signal * 7;
    oscFilter.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue());
    oscFilter.Process(oscSignal);
    oscSignal = oscFilter.Band() * ampEnvSignal * 2;

    float noiseEnvSignal = noiseEnv.Process();
    float noiseSignal = noiseFilter.Process(noise.Process());   
    noiseSignal = noiseSignal * noiseEnvSignal * 0.5;   

    active = ampEnv.IsRunning() || noiseEnv.IsRunning();
    float signal = oscSignal + parameters[PARAM_REVERB_LEVEL].GetScaledValue() * noiseSignal; 
    signal = Utility::Squeeze(signal, parameters[PARAM_SQUEEZE].GetScaledValue());
    return signal * velocity;
}

void Tom8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        active = true;
        ampEnv.Trigger();
        pitchEnv.Trigger();
        noiseEnv.Trigger();
    }
}

float Tom8::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string Tom8::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_FREQUENCY2: 
                return std::to_string((int)GetParam(param));
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_OSC2_LEVEL: 
            case PARAM_RINGMOD_LEVEL: 
            case PARAM_REVERB_LEVEL: 
            case PARAM_BEND: 
                return std::to_string((int)(GetParam(param) * 100));
            case PARAM_SQUEEZE:
                return std::to_string((int)(GetParam(param) * 4));
        }
    }
   return "";
 }

float Tom8::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_FREQUENCY2: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 20, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_AMP_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                noiseEnv.SetTime(ADENV_SEG_DECAY, scaled * 1.5);
                break;
            case PARAM_OSC2_LEVEL: 
            case PARAM_RINGMOD_LEVEL: 
            case PARAM_REVERB_LEVEL: 
            case PARAM_BEND: 
                scaled = parameters[param].Update(raw, Utility::LimitFloat(raw, 0, 1));
                break;
            case PARAM_SQUEEZE:
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 25, Parameter::EXPONENTIAL));

        }
    }

    return scaled;    
}

void Tom8::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Tom8::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_FREQUENCY2: 
            case PARAM_OSC2_LEVEL: 
            case PARAM_RINGMOD_LEVEL: 
            case PARAM_SQUEEZE:
            case PARAM_REVERB_LEVEL: 
            case PARAM_BEND: 
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_AMP_DECAY: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                noiseEnv.SetTime(ADENV_SEG_DECAY, scaled * 1.5);
                break;
        }
    }
}

void Tom8::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            if (Slot() == "MT") {
                SetParam(param, mtPresets[preset][param]);
            } else if (Slot() == "HT") {
                SetParam(param, htPresets[preset][param]);
            } else {
                SetParam(param, presets[preset][param]);
            }
        }
    }
}
