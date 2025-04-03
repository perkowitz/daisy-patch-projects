#include "FmDrum.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float FmDrum::presets[][FmDrum::PARAM_COUNT] = {
    // freq, ratio, mod_amt, dcy, atk, curve
    {98, 3.330, 2.220, 0.101, 0.005, -50},
    {98, 1.191, 0.350, 0.101, 0.005, -50},
    {932, 1.140, 0.460, 0.131, 0.005, -50},
    {340, 1.123, 0.13, 0.220, 0.005, -50},
    {196, 71714, 0.410, 0.040, 0.165, -50},
    {25, 0.31, 0.31, 1.172, 0.005, -50},
    {26, 0.13, 0.20, 1.559, 0.005, -50},
    {123, 0.60, 0.60, 1.1134, 0.088, -50}
};

float FmDrum::hcPresets[][FmDrum::PARAM_COUNT] = {
    // freq, ratio, mod_amt, dcy, atk, curve
    {131, 3.330, 2.220, 0.101, 0.005, -50},
    {131, 2.202, 0.330, 0.101, 0.005, -50},
    {1667, 0.75, 0.37, 0.069, 0.005, -50},
    {424, 0.59, 0.50, 0.234, 0.005, -50},
    {254, 5.556, 0.61, 0.020, 0.195, -50},
    {2077, 0.13, 0.39, 0.577, 0.005, -50},
    {4727, 0.16, 1.144, 0.05, 0.101, -50},
    {169, 0.41, 0.39, 0.886, 0.005, -50}
};


void FmDrum::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 68, 3.3, 2.2, 0.001, 0.043, -50);
}

void FmDrum::Init(std::string slot, float sample_rate, float frequency, float ratio, float modAmount, float attack, float decay, float curve) {

    this->slot = slot;
    
    // 2-osc fm object
    fm.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    SetParam(PARAM_RATIO, ratio);
    SetParam(PARAM_MOD_AMT, modAmount);

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_DECAY, decay);
    SetParam(PARAM_ENV_CURVE, curve);
}

float FmDrum::Process() {
    active = ampEnv.IsRunning();
    return velocity * fm.Process() * ampEnv.Process();
}

void FmDrum::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        fm.Reset();
        active = true;
        ampEnv.Trigger();
    }
}

float FmDrum::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string FmDrum::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                return std::to_string((int)GetParam(param));// + "hz";
            case PARAM_RATIO: 
                return Utility::FloatToString2(GetParam(param));
            case PARAM_MOD_AMT: 
                return Utility::FloatToString2(GetParam(param));
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_ENV_CURVE:
                return std::to_string((int)GetParam(param));
        }
    }
   return "";
 }

float FmDrum::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 20, 5000, Parameter::EXPONENTIAL));
                fm.SetFrequency(scaled);
                break;
            case PARAM_RATIO: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.125, 8, Parameter::EXPONENTIAL));
                fm.SetRatio(scaled);
                break;
            case PARAM_MOD_AMT: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 5, Parameter::EXPONENTIAL));
                fm.SetIndex(scaled);
                break;
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_ENV_CURVE: 
                scaled = raw;
                // TODO: set the curve
                break;
        }
    }

    return scaled;    
}

void FmDrum::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void FmDrum::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                parameters[param].SetScaledValue(scaled);
                fm.SetFrequency(scaled);
                break;
            case PARAM_RATIO: 
                parameters[param].SetScaledValue(scaled);
                fm.SetRatio(scaled);
                break;
            case PARAM_MOD_AMT: 
                parameters[param].SetScaledValue(scaled);
                fm.SetIndex(scaled);
                break;
            case PARAM_ATTACK: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_ENV_CURVE: 
                parameters[param].SetScaledValue(scaled);
                // TODO: set the curve
                break;
        }
    }
}

void FmDrum::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            if (Slot() == "HC") {
                SetParam(param, hcPresets[preset][param]);
            } else {
                SetParam(param, presets[preset][param]);
            }
        }
    }
}
