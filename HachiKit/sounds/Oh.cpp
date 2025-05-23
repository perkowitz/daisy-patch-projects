#include "Oh.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


// OH presets don't track the source params that CH already tracks (so only 3 params)
float Oh::presets[][3] = {
    // atk, hold, dcy
    {0.005, 0.130, 0.05},
    {0.080, 0, 0.200},
    {0.005, 0.130, 0.05},
    {0.005, 0.130, 0.05},
    {0.005, 0.130, 0.05},
    {0.005, 0.130, 0.05},
    {0.005, 0.130, 0.05},
    {0.005, 0.130, 0.05}
};


void Oh::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.001, 0.13f, 0.05f, NULL, 0.5, 2000, 5000);
}

void Oh::Init(std::string slot, float sample_rate, float attack, float hold, float decay, HhSource68 *source, float morph, float hpf, float lpf) {

    this->slot = slot;
    
    // env settings
    env.Init(sample_rate);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_HOLD, hold);
    SetParam(PARAM_DECAY, decay);

    // source settings
    this->source = source;
    SetParam(PARAM_MORPH, morph);
    SetParam(PARAM_HPF, hpf);
    SetParam(PARAM_LPF, lpf);

}

float Oh::Process() {
    if (source == NULL) {
        return 0.0f;
    }

    float sig = source->Signal() * env.Process();
    active = env.IsRunning();
    return velocity * sig;
}

void Oh::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        active = true;
        env.Trigger();
    }
}

float Oh::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_HOLD: 
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

std::string Oh::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_HOLD: 
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

float Oh::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.001, 5, Parameter::EXPONENTIAL));
                env.SetAttack(scaled);
                break;
            case PARAM_HOLD: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.001, 5, Parameter::EXPONENTIAL));
                env.SetHold(scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.001, 5, Parameter::EXPONENTIAL));
                env.SetDecay(scaled);
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

void Oh::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
    source->ResetParams();
}

void Oh::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                parameters[param].SetScaledValue(scaled);
                env.SetAttack(scaled);
                break;
            case PARAM_HOLD: 
                parameters[param].SetScaledValue(scaled);
                env.SetHold(scaled);
                break;
            case PARAM_DECAY: 
                parameters[param].SetScaledValue(scaled);
                env.SetDecay(scaled);
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

// OH presets don't track the source params that CH already tracks (so only 3 params)
void Oh::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < 3; param++) {
            SetParam(param, presets[preset][param]);
        }
    }
}
