#include "Tom.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float Tom::presets[][Tom::PARAM_COUNT] = {
    // freq, adcy, pmod, lpf-mod, hpf, lpf 
    {80, 1.100, 60, 116, 1500, 191},
    {197, 1.297, 160, 168, 154, 230},
    {298, 0.593, 81, 259, 1646, 191},
    {495, 0.579, 62, 349, 1417, 359},
    {77, 1.926, 245, 364, 195, 191},
    {506, 1.812, 131, 364, 1079, 191},
    {171, 0.682, 2, 263, 482, 191},
    {954, 0.449, 2, 2, 773, 900}
};


void Tom::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 80, NULL);
}

void Tom::Init(std::string slot, float sample_rate, float frequency, ClickSource *clickSource) {

    this->slot = slot;
    this->clickSource = clickSource;

    osc.Init(sample_rate);
    SetParam(PARAM_FREQUENCY, frequency);
    osc.SetWaveform(Oscillator::WAVE_TRI);

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-10);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, 1.1);
    SetParam(PARAM_PITCH_MOD, 60);

    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    pitchEnv.SetCurve(-20);
    pitchEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    pitchEnv.SetTime(ADENV_SEG_DECAY, 0.25);
}

float Tom::Process() {

    float clickSignal = clickSource == NULL ? 0.0f :  clickSource->Signal();
    
    // sine osc freq is modulated by pitch env, amp by amp env
    float pitchEnvSignal = pitchEnv.Process();
    float ampEnvSignal = ampEnv.Process();
    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + parameters[PARAM_PITCH_MOD].GetScaledValue() * pitchEnvSignal);
    float oscSignal = osc.Process() * ampEnvSignal;

    // TODO: figure out why this wasn't working
    // // // apply velocity scaling more strong to noise than osc
    // // float signal = noiseSignal * velocity + oscSignal * (0.4 + velocity * 0.6);
    // // return signal;
    active = ampEnv.IsRunning() || clickSource->IsActive();
    return (clickSignal + oscSignal) * velocity; 
}

void Tom::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        active = true;
        clickSource->Trigger(velocity);
        ampEnv.Trigger();
        pitchEnv.Trigger();
    }
}

float Tom::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_AMP_DECAY: 
            case PARAM_PITCH_MOD:
                return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
            case PARAM_LPF_MOD:
                return clickSource->GetParam(ClickSource::PARAM_LPF_MOD);
            case PARAM_HPF:
                return clickSource->GetParam(ClickSource::PARAM_HPF);
            case PARAM_LPF:
                return clickSource->GetParam(ClickSource::PARAM_LPF);
        }
    }

    return 0.0f;
}

std::string Tom::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_PITCH_MOD:
                return std::to_string((int)GetParam(param));
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
            case PARAM_LPF_MOD:
                return clickSource->GetParamString(ClickSource::PARAM_LPF_MOD);
            case PARAM_HPF:
                return clickSource->GetParamString(ClickSource::PARAM_HPF);
            case PARAM_LPF:
                return clickSource->GetParamString(ClickSource::PARAM_LPF);
        }
    }
   return "";
 }

float Tom::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 20, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_AMP_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_PITCH_MOD:
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_LPF_MOD:
                clickSource->UpdateParam(ClickSource::PARAM_LPF_MOD, raw);
                break;
            case PARAM_HPF:
                clickSource->UpdateParam(ClickSource::PARAM_HPF, raw);
                break;
            case PARAM_LPF:
                clickSource->UpdateParam(ClickSource::PARAM_LPF, raw);
                break;
        }
    }

    return scaled;    
}

void Tom::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Tom::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_AMP_DECAY: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                break;
            case PARAM_PITCH_MOD:
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_LPF_MOD:
                clickSource->SetParam(ClickSource::PARAM_LPF_MOD, scaled);
                break;
            case PARAM_HPF:
                clickSource->SetParam(ClickSource::PARAM_LPF_MOD, scaled);
                break;
            case PARAM_LPF:
                clickSource->SetParam(ClickSource::PARAM_LPF_MOD, scaled);
                break;
        }
    }
}

void Tom::LoadPreset(u8 preset) {
    float ratio = 1.0;
    if (preset < IDRUM_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            if (param == PARAM_FREQUENCY) {
                if (Slot() == "MT") {
                    ratio = 6.0 / 5.0;
                    if (param % 2 == 1) {
                        ratio = 5.0 / 4.0;
                    }
                    SetParam(param, presets[preset][param] * ratio);
                } else if (Slot() == "HT") {
                    ratio = 3.0 / 2.0;
                    if (param % 2 == 1) {
                        ratio = 4.0 / 3.0;
                    }
                    SetParam(param, presets[preset][param] * ratio);
                } else { // if LT
                    SetParam(param, presets[preset][param]);
                }
            } else {
                SetParam(param, presets[preset][param]);
            }
        }
    } 
}
