#include "Ch.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float Ch::presets[][Ch::PARAM_COUNT] = {
    // atk, dcy, morph, hpf, lpf
    {0.005, 0.297, 0.65, 2500, 16000},
    {0.005, 0.239, 0.37, 2200, 16000},
    {0.005, 0.646, 0.71, 1634, 16000},
    {0.005, 0.584, 0.25, 1839, 2650},
    {0.110, 0.947, 0.03, 1864, 7900},
    {0.005, 0.396, 0.89, 1240, 13000},
    {0.1, 0.200, 0.45, 1900, 10000},
    {0.005, 0.716, 0.58, 2700, 15044}
};


void Ch::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.001, 0.5, NULL, 0.5, 2000, 5000);
}

void Ch::Init(std::string slot, float sample_rate, float attack, float decay, HhSource68 *source, float morph, float hpf, float lpf) {

    this->slot = slot;

    // env settings
    env.Init(sample_rate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(-20);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_DECAY, decay);

    // source settings
    this->source = source;
    SetParam(PARAM_MORPH, morph);
    SetParam(PARAM_HPF, hpf);
    SetParam(PARAM_LPF, lpf);

}

float Ch::Process() {
    if (source == NULL) {
        return 0.0f;
    }

    active = env.IsRunning();
    float sig = source->Signal() * env.Process();
    return velocity * sig;
}

void Ch::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        active = true;
        env.Trigger();
    }
}

float Ch::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
                return parameters[param].GetScaledValue();
            case PARAM_MORPH:
                return source->GetParam(HhSource68::PARAM_MORPH);
            case PARAM_HPF:
                return source->GetParam(HhSource68::PARAM_HPF);
            case PARAM_LPF:
                return source->GetParam(HhSource68::PARAM_LPF);
        }
    }
    return 0.0f;
}

std::string Ch::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_MORPH:
                return std::to_string((int)(GetParam(param) * 100));
            case PARAM_HPF:
            case PARAM_LPF:
                return std::to_string((int)GetParam(param));
        }
    }
   return "";
 }

float Ch::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                env.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                env.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_MORPH:
                source->UpdateParam(HhSource68::PARAM_MORPH, raw);
                break;
            case PARAM_HPF:
                source->UpdateParam(HhSource68::PARAM_HPF, raw);
                break;
            case PARAM_LPF:
                source->UpdateParam(HhSource68::PARAM_LPF, raw);
                break;
        }
    }

    return scaled;    
}

void Ch::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
    source->ResetParams();
}

void Ch::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                parameters[param].SetScaledValue(scaled);
                env.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                parameters[param].SetScaledValue(scaled);
                env.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_MORPH:
                source->SetParam(HhSource68::PARAM_MORPH, scaled);
                break;
            case PARAM_HPF:
                source->SetParam(HhSource68::PARAM_HPF, scaled);
                break;
            case PARAM_LPF:
                source->SetParam(HhSource68::PARAM_LPF, scaled);
                break;
        }
    }
}

void Ch::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param]);
        }
    }
}
    