#include "Clap.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

float Clap::presets[][Clap::PARAM_COUNT] = {
    // spread, decay
    {0.012, 0.800},
    {0.008, 0.440},
    {0.023, 0.490},
    {0.049, 0.202},
    {0.010, 0.694},
    {0.012, 0.102},
    {0.016, 0.954},
    {0.010, 1.515}
};

    void Clap::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.012, 1.0);
}

void Clap::Init(std::string slot, float sample_rate, float spread, float decay) {

    this->slot = slot;

    noise.Init();

    env.Init(sample_rate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(-20);
    env.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_DECAY, decay);
    SetParam(PARAM_SPREAD, spread);
}

float Clap::Process() {
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
    return 3 * velocity * noise.Process() * env.Process();
}

void Clap::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        repeat = 0;
        active = true;
        env.SetTime(ADENV_SEG_DECAY, GetParam(PARAM_SPREAD));
        env.Trigger();
    }
}

float Clap::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string Clap::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_SPREAD: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
        }
    }
   return "";
 }

float Clap::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true);
}

void Clap::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Clap::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float Clap::SetParam(uint8_t param, float value, bool isRaw) {
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
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 3, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
        }
    }

    return 0.0f;
}

void Clap::LoadPreset(u8 preset) {
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            SetParam(param, presets[preset][param], false);   // isRaw=false will directly set to the provided value
        }
    }
}
