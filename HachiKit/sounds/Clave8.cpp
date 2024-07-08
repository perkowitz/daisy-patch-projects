#include "Clave8.h"

using namespace daisy;
using namespace daisysp;

void Clave8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 2000, 0.375);
}

void Clave8::Init(std::string slot, float sample_rate, float frequency, float ampDecay) {

    this->slot = slot;

    // oscillator settings
    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_TRI);

    // ampEnv settings
    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-20);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, ampDecay);

    // band-pass filter
    bpf.Init(sample_rate);
    bpf.SetRes(0.2);
    bpf.SetDrive(.1);

    SetParam(PARAM_FREQUENCY, frequency);
}

float Clave8::Process() {
    if (!active) return 0.0f; 

    active = ampEnv.IsRunning();
    bpf.Process(osc.Process());
    return velocity * bpf.Band() * ampEnv.Process() * 3;
}

void Clave8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        active = true;
        ampEnv.Trigger();
    }
}

float Clave8::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string Clave8::GetParamString(uint8_t param) {
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

float Clave8::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true); 
}

void Clave8::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Clave8::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float Clave8::SetParam(uint8_t param, float value, bool isRaw) {
    float scaled = value;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 100, 3000, Parameter::LINEAR));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                osc.SetFreq(scaled);
                bpf.SetFreq(scaled);
                return scaled;
            case PARAM_AMP_DECAY:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 5, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                return scaled;
        }
    }

    return 0.0f;
}