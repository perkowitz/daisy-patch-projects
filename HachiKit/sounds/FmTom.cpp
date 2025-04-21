#include "FmTom.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float FmTom::presets[][FmTom::PARAM_COUNT] = {
    // freq, dcy, ratio, mix
    {150, 1.100, 1418, 0.5},
    {226, 0.571, 131, 0.35},
    {56, 0.782, 768, 0.95},
    {78, 0.690, 747, 0.48},
    {59, 0.853, 101, 0.60},
    {22, 0.585, 2, 0.03},
    {171, 0.708, 1944, 0.04},
    {70, 1.824, 394, 0.95}
};

float FmTom::mtPresets[][FmTom::PARAM_COUNT] = {
    // freq, dcy, ratio, mix
    {presets[0][0] * 6 / 5, 1.100, 787, 0.5},
    {325, 0.919, 258, 0.30},
    {77, 0.894, 1890, 0.91},
    {232, 1.079, 701, 0.52},
    {71, 0.850, 101, 0.42},
    {39, 0.900, 8, 0.03},
    {344, 0.488, 3, 0.05},
    {93, 0.649, 1018, 0.90}
};

float FmTom::htPresets[][FmTom::PARAM_COUNT] = {
    // freq, dcy, ratio, mix
    {presets[0][0] * 3 / 2, 1.100, 1212, 0.5},
    {667, 0.663, 1250, 0.55},
    {202, 0.642, 216, 0.87},
    {266, 0.493, 307, 0.39},
    {153, 0.617, 101, 0.37},
    {59, 1.287, 46, 0.93},
    {447, 0.824, 1945, 0.04},
    {120, 0.540, 1941, 0.93}
};


void FmTom::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 80);
}

void FmTom::Init(std::string slot, float sample_rate, float frequency) {

    this->slot = slot;

    osc.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    osc.SetWaveform(Oscillator::WAVE_TRI);

    fmOsc.Init(sample_rate);

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-10);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, 1.1);
    SetParam(PARAM_RATIO, 60);

    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    pitchEnv.SetCurve(-10);
    pitchEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    pitchEnv.SetTime(ADENV_SEG_DECAY, 0.25);
}

float FmTom::Process() {
  
    // sine osc freq is modulated by pitch env, amp by amp env
    float pitchEnvSignal = pitchEnv.Process();
    float ampEnvSignal = ampEnv.Process();
    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + parameters[PARAM_FREQUENCY].GetScaledValue() * 0.2 * pitchEnvSignal);
    float oscSignal = 2 * parameters[PARAM_MIX].GetScaledValue() * osc.Process() * ampEnvSignal;

    fmOsc.SetFrequency(parameters[PARAM_FREQUENCY].GetScaledValue());
    fmOsc.SetRatio(parameters[PARAM_RATIO].GetScaledValue() / 100);
    fmOsc.SetIndex(1);
    float fmSignal = (1 - parameters[PARAM_MIX].GetScaledValue()) * fmOsc.Process() * ampEnvSignal;

    active = ampEnv.IsRunning();
    return (oscSignal + fmSignal) * velocity; 
}

void FmTom::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        fmOsc.Reset();
        active = true;
        ampEnv.Trigger();
        pitchEnv.Trigger();
    }
}

float FmTom::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string FmTom::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_RATIO:
                return std::to_string((int)GetParam(param));
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_MIX:
                return std::to_string((int)(GetParam(param) * 100));
        }
    }
   return "";
 }

float FmTom::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 20, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_AMP_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_RATIO:
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_MIX:
                scaled = parameters[param].Update(raw, Utility::LimitFloat(raw, 0, 1));
                break;
        }
    }

    return scaled;    
}

void FmTom::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void FmTom::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_AMP_DECAY: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_RATIO:
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_MIX: 
                parameters[param].SetScaledValue(scaled);
                break;
        }
    }
}

void FmTom::LoadPreset(u8 preset) {
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
