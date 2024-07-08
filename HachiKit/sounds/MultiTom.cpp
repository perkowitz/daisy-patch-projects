#include "MultiTom.h"

using namespace daisy;
using namespace daisysp;

void MultiTom::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 200, 0.5, NULL, 0);
}

void MultiTom::Init(std::string slot, float sample_rate, float frequency, float ampDecay, MultiTomSource *multiTomSource, u8 index) {

    this->slot = slot;
    this->multiTomSource = multiTomSource;
    this->index = index;

    SetParam(PARAM_FREQUENCY, frequency);
    SetParam(PARAM_AMP_DECAY, ampDecay);
}

float MultiTom::Process() {
    if (!active) return 0.0f; 

    active = multiTomSource->IsActive();
    return multiTomSource->Signal();
}

void MultiTom::Trigger(float velocity) {
    multiTomSource->Trigger(velocity, index, GetParam(PARAM_FREQUENCY));
}

float MultiTom::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                return parameters[param].GetScaledValue();
            case PARAM_AMP_DECAY:
                return multiTomSource->GetParam(MultiTomSource::PARAM_AMP_DECAY);
        }
    }
    return 0.0f;
}

std::string MultiTom::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                return std::to_string((int)GetParam(param));// + "hz";
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
        }
    }
   return "";
 }

float MultiTom::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true); 
}

void MultiTom::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void MultiTom::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float MultiTom::SetParam(uint8_t param, float value, bool isRaw) {
    float scaled = value;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 20, 5000, Parameter::LINEAR));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                return scaled;
            case PARAM_AMP_DECAY:
                if (isRaw) {
                    return multiTomSource->UpdateParam(MultiTomSource::PARAM_AMP_DECAY, value);
                } else {
                    multiTomSource->SetParam(MultiTomSource::PARAM_AMP_DECAY, value);
                }
                return scaled;
        }
    }

    return 0.0f;
}