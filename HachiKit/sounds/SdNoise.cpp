#include "SdNoise.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

void SdNoise::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 0.01, 0.237, -30.0f);
}

void SdNoise::Init(std::string slot, float sample_rate, float attack, float decay, float curve) {

    this->slot = slot;
    noise.Init();

    env.Init(sample_rate);
    env.SetStageTime(AhdEnv::STAGE_HOLD, 0);
    SetParam(PARAM_ATTACK, attack);
    SetParam(PARAM_DECAY, decay);
    SetParam(PARAM_CURVE, curve);
}

float SdNoise::Process() {
    active = env.IsRunning();
    return velocity * noise.Process() * env.Process();
}

void SdNoise::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        active = true;
        env.Trigger();
    }
}

float SdNoise::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string SdNoise::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
            case PARAM_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_CURVE: 
                return std::to_string((int)GetParam(param));
        }
    }
   return "";
 }

float SdNoise::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 5, Parameter::EXPONENTIAL));
                env.SetStageTime(AhdEnv::STAGE_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 5, Parameter::EXPONENTIAL));
                env.SetStageTime(AhdEnv::STAGE_DECAY, scaled);
                break;
            case PARAM_CURVE: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 10, Parameter::LINEAR));
                env.SetCurve(scaled);
                break;
        }
    }

    return scaled;    
}

void SdNoise::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void SdNoise::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_ATTACK: 
                parameters[param].SetScaledValue(scaled);
                env.SetStageTime(AhdEnv::STAGE_ATTACK, scaled);
                break;
            case PARAM_DECAY: 
                parameters[param].SetScaledValue(scaled);
                env.SetStageTime(AhdEnv::STAGE_DECAY, scaled);
                break;
            case PARAM_CURVE: 
                parameters[param].SetScaledValue(scaled);
                env.SetCurve(scaled);
                break;
        }
    }    
}
