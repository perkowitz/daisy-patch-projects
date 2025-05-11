#include "Cy.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


const float Cy::HPF_MAX = 12000;
const float Cy::HPF_MIN = 100;
const float Cy::LPF_MAX = 18000;
const float Cy::LPF_MIN = 100;


float Cy::presets[][Cy::PARAM_COUNT] = {
    // atk, dcy, hpf, lpf
    {0.005, 3.500, 1700, 2400},
    {0.005, 2.300, 2406, 3955},
    {0.100, 7.686, 3553, 1919},
    {0.320, 4.181, 732, 1137},
    {0.005, 5.800, 5391, 5068},
    {1.487, 0.004, 1005, 6039},
    {0.005, 5.820, 1019, 1630},
    {0.505, 0.505, 573, 3696}
};


void Cy::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.001, 3.5, NULL, 1700, 2300);
}

void Cy::Init(std::string slot, float sample_rate, float attack, float decay, HhSource68 *source, float hpfCutoff, float lpfCutoff) {

    this->slot = slot;

    // env settings
    env.Init(sample_rate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(-4);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_DECAY, decay);

    hpf.Init(sample_rate);
    hpf.SetRes(0.2);
    hpf.SetDrive(.002);
    SetParam(PARAM_HPF, hpfCutoff);

    lpf.Init(sample_rate);
    lpf.SetRes(0.2);
    lpf.SetDrive(.002);
    SetParam(PARAM_LPF, lpfCutoff);

    this->source = source;

}

float Cy::Process() {
    if (source == NULL) {
        return 0.0f;
    }

    float sig = 0.0f;
    // if (bufferValid) {
    //     if (bufferIndex < BUFFER_SIZE) {
    //         sig = buffer[bufferIndex];
    //     }
    // } else {
        sig = source->Signal() * env.Process();
        hpf.Process(sig);
        lpf.Process(hpf.High());
        sig = lpf.Low();
        // buffer[bufferIndex] = sig;
    // }
    // bufferIndex++;

    active = env.IsRunning();
    return velocity * sig;
}

void Cy::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        active = true;
        env.Trigger();
        // bufferIndex = 0;
    }
}

float Cy::GetParam(uint8_t param) {
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

std::string Cy::GetParamString(uint8_t param) {
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

float Cy::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                env.SetTime(ADENV_SEG_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 15, Parameter::EXPONENTIAL));
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

void Cy::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
    source->ResetParams();
}

void Cy::SetParam(uint8_t param, float scaled) {
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
    }
}

void Cy::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param]);
        }
    }
}
    