#include "Cow8.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


const float Cow8::HPF_MAX = 12000;
const float Cow8::HPF_MIN = 100;
const float Cow8::LPF_MAX = 18000;
const float Cow8::LPF_MIN = 100;


float Cow8::presets[][Cow8::PARAM_COUNT] = {
    // atk, dcy, hpf, lpf
    {0.005, 0.500, 901, 1681},
    {0.005, 0.817, 266, 2029},
    {0.300, 0.005, 1181, 7000},
    {0.005, 0.878, 1862, 2675},
    {0.265, 0.240, 473, 2124},
    {0.020, 2.324, 1431, 1026},
    {0.005, 1.816, 1834, 3309},
    {0.005, 1.605, 215, 9107}
};


void Cow8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.001, 3.5, NULL, 1700, 2300);
}

void Cow8::Init(std::string slot, float sample_rate, float attack, float decay, HhSource68 *source, float hpfCutoff, float lpfCutoff) {

    this->slot = slot;

    // env settings
    env.Init(sample_rate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(-20);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_DECAY, decay);

    hpf.Init(sample_rate);
    hpf.SetRes(0.2);
    hpf.SetDrive(.002);
    SetParam(PARAM_HPF, hpfCutoff);

    lpf.Init(sample_rate);
    lpf.SetRes(0.2);
    lpf.SetDrive(.002);
    lpf.SetFreq(lpfCutoff);
    SetParam(PARAM_LPF, lpfCutoff);

    this->source = source;

}

float Cow8::Process() {
    if (source == NULL) {
        return 0.0f;
    }

    float sig = source->Cowbell(false) * env.Process();
    hpf.Process(sig);
    lpf.Process(hpf.High());
    active = env.IsRunning();
    return velocity * lpf.Low();;
}

void Cow8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        active = true;
        env.Trigger();
    }
}

float Cow8::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
            case PARAM_HPF:
            case PARAM_LPF:
                return parameters[param].GetScaledValue();
        }
    }
    return 0.0f;
}

std::string Cow8::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_HPF:
            case PARAM_LPF:
                return std::to_string((int)GetParam(param));
        }
    }
   return "";
 }

float Cow8::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.001, 5, Parameter::EXPONENTIAL));
                env.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.001, 15, Parameter::EXPONENTIAL));
                env.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_HPF:
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, HPF_MIN, HPF_MAX, Parameter::EXPONENTIAL));
                hpf.SetFreq(scaled);
                break;
            case PARAM_LPF:
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, LPF_MIN, LPF_MAX, Parameter::EXPONENTIAL));
                lpf.SetFreq(scaled);
                break;
        }
    }

    return scaled;    
}

void Cow8::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
    source->ResetParams();
}

void Cow8::SetParam(uint8_t param, float scaled) {
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
            case PARAM_HPF:
                parameters[param].SetScaledValue(scaled);
                hpf.SetFreq(scaled);
                break;
            case PARAM_LPF:
                parameters[param].SetScaledValue(scaled);
                lpf.SetFreq(scaled);
                break;
        }
    }}

void Cow8::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param]);
        }
    }
}
    