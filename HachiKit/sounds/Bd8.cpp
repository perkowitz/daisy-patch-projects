#include "Bd8.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

void Bd8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 78, 0.001, 4.0, 0.001, 0.1, 0.95);
}

void Bd8::Init(std::string slot, float sample_rate, float frequency, float ampAttack, float ampDecay, float pitchAttack, float pitchDecay, float modAmount) {

    this->slot = slot;

    // oscillator settings
    osc.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    osc.SetWaveform(Oscillator::WAVE_SIN);

    // ampEnv settings
    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    SetParam(PARAM_AMP_ATTACK, ampAttack);
    SetParam(PARAM_AMP_DECAY, ampDecay);

    // pitchEnv settings
    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    SetParam(PARAM_PITCH_ATTACK, pitchAttack);
    SetParam(PARAM_PITCH_DECAY, pitchDecay);
    SetParam(PARAM_MOD_AMT, modAmount);
    SetParam(PARAM_AMP_CURVE, -30);
}

float Bd8::Process() {
    float psig = pitchEnv.Process();
    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + parameters[PARAM_MOD_AMT].GetScaledValue() * psig);
    // osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue());
    return 6 * velocity * osc.Process() * ampEnv.Process();
}

void Bd8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
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
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.01, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                ampEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_AMP_DECAY: 
                if (isRaw) {
                  scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.01, 10, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_PITCH_ATTACK: 
                if (isRaw) {
                 scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.01, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                pitchEnv.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_PITCH_DECAY: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.01, 10, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                pitchEnv.SetTime(ADENV_SEG_DECAY, scaled);
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
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, -100, 100, Parameter::LINEAR));
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

