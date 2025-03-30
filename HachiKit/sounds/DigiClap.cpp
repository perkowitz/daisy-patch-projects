#include "DigiClap.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

float DigiClap::presets[][DigiClap::PARAM_COUNT] = {
    // spread, decay, freq, mod
    {0.011, 3.386, 5547, 2471},
    {0.009, 3.736, 2328, 1538},
    {0.002, 4.724, 1597, -455},
    {0.002, 1.037, 3922, 3765},
    {0.005, 4.850, 317, 1004},
    {0.012, 2.125, 5290, -141},
    {0.017, 4.643, 7616, -5064},
    {0.011, 0.746, 1542, 800}
};


void DigiClap::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.012f, 1.0f, 3000.0f, 0);
}

void DigiClap::Init(std::string slot, float sample_rate, float spread, float decay, float frequency, float mod) {

    this->slot = slot;

    clockedNoise.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    SetParam(PARAM_MOD, mod);

    env.Init(sample_rate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(-20);
    env.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_DECAY, decay);
    SetParam(PARAM_SPREAD, spread);
}

float DigiClap::Process() {
    if (!env.IsRunning()) {
        repeat++;
        if (repeat == REPEATS) {
            env.SetTime(ADENV_SEG_DECAY, GetParam(PARAM_DECAY));
        }
        if (repeat <= REPEATS) {
            env.Trigger();
        }
    }
    if (repeat <= REPEATS) {
        active = true;
    } else {
        active = env.IsRunning(); 
    }
    float envSig = env.Process();
    clockedNoise.SetFreq(GetParam(PARAM_FREQUENCY) + GetParam(PARAM_MOD) * envSig);
    return velocity * clockedNoise.Process() * envSig;
}

void DigiClap::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        repeat = 0;
        env.SetTime(ADENV_SEG_DECAY, GetParam(PARAM_SPREAD));
        active = true;
        env.Trigger();
    }
}

float DigiClap::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string DigiClap::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_SPREAD: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_FREQUENCY: 
                return std::to_string((int)(GetParam(param)));
            case PARAM_MOD: 
                return std::to_string((int)(GetParam(param)));
        }
    }
   return "";
 }

float DigiClap::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true);
}

void DigiClap::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void DigiClap::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float DigiClap::SetParam(uint8_t param, float value, bool isRaw) {
    float scaled = value;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_SPREAD:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 0.050, Parameter::LINEAR));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
            case PARAM_DECAY: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
            case PARAM_FREQUENCY: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 20, 15000, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
            case PARAM_MOD: 
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, -15000, 15000, Parameter::LINEAR));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
        }
    }

    return 0.0f;
}

void DigiClap::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param], false);   // isRaw=false will directly set to the provided value
        }
    } 
}
