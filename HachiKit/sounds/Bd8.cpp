#include "Bd8.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

float Bd8::presets[][Bd8::PARAM_COUNT] = {
    // freq, mod, adcy, pdcy, aatk, patk, curv 
    {68, 231, 2.174, 0.02, 0, 0, 4},
    {58, 1932, 3.473, 0.01, 0, 0, 4},
    {72, 340, 1.355, 0.073, 0, 0, 3},
    {75, 577, 3.120, 0.032, 0, 0, 5},
    {66, 1691, 0.954, 0.02, 0, 0, 4},
    {54, 620, 0.360, 0.011, 0, 0, 1},
    {285, 40, 0.183, 0.02, 0, 0, 1},
    {69, 1934, 5.105, 0.02, 0, 0, 6}
};


void Bd8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 78, 1.01, 4.0, 1.01, 0.1, 0.95);
}

void Bd8::Init(std::string slot, float sample_rate, float frequency, float ampAttack, float ampDecay, float pitchAttack, float pitchDecay, float modAmount) {

    this->slot = slot;

    // oscillator settings
    osc.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    osc.SetWaveform(Oscillator::WAVE_SIN);

    // ampEnv settings
    ampEnv.Init(sample_rate);
    ampEnv.SetStageTime(AhdEnv::STAGE_HOLD, 0);
    SetParam(PARAM_AMP_ATTACK, ampAttack);
    SetParam(PARAM_AMP_DECAY, ampDecay);

    // pitchEnv settings
    pitchEnv.Init(sample_rate);
    pitchEnv.SetStageTime(AhdEnv::STAGE_HOLD, 0);
    SetParam(PARAM_PITCH_ATTACK, pitchAttack);
    SetParam(PARAM_PITCH_DECAY, pitchDecay);

    SetParam(PARAM_MOD_AMT, modAmount);
    SetParam(PARAM_AMP_CURVE, 4);
}

float Bd8::Process() {
    float psig = pitchEnv.Process();
    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + parameters[PARAM_MOD_AMT].GetScaledValue() * psig);
    active = ampEnv.IsRunning();
    return 6 * velocity * osc.Process() * ampEnv.Process();
}

void Bd8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        active = true;
        ampEnv.Trigger();
        pitchEnv.Trigger();
    }
}

float Bd8::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string Bd8::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_MOD_AMT: 
                return std::to_string((int)GetParam(param));// + "hz";
            case PARAM_AMP_ATTACK: 
            case PARAM_AMP_DECAY: 
            case PARAM_PITCH_ATTACK: 
            case PARAM_PITCH_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_AMP_CURVE: 
                return std::to_string((int)(GetParam(param)));
        }
    }
   return "";
 }

float Bd8::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true); 
}

void Bd8::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Bd8::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float Bd8::SetParam(uint8_t param, float value, bool isRaw) {
    float scaled = value;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 20, 5000, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                break;
            case PARAM_AMP_ATTACK: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                // ampEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                ampEnv.SetStageTime(AhdEnv::STAGE_ATTACK, scaled);
                break;
            case PARAM_AMP_DECAY: 
                if (isRaw) {
                  scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 10, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                // ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                ampEnv.SetStageTime(AhdEnv::STAGE_DECAY, scaled);
                break;
            case PARAM_PITCH_ATTACK: 
                if (isRaw) {
                 scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                // pitchEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                pitchEnv.SetStageTime(AhdEnv::STAGE_ATTACK, scaled);
                break;
            case PARAM_PITCH_DECAY: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 10, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                // pitchEnv.SetTime(ADENV_SEG_DECAY, scaled);
                pitchEnv.SetStageTime(AhdEnv::STAGE_DECAY, scaled);
                break;
            case PARAM_MOD_AMT: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0, 2000, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(scaled);
                }
                break;
            case PARAM_AMP_CURVE:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0, 10, Parameter::LINEAR));
                } else {
                    parameters[param].SetScaledValue(scaled);
                }
                ampEnv.SetCurve(scaled);
                pitchEnv.SetCurve(scaled);
                break;
        }
    }

    return 0.0f;
}

void Bd8::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param], false);   // isRaw=false will directly set to the provided value
        }
    }
}

