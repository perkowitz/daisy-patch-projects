#include "Tom8.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


float Tom8::presets[][Tom8::PARAM_COUNT] = {
    // freq, decay
    {80, 1.100},
    {197, 1.297},
    {298, 0.593},
    {495, 0.579},
    {77, 1.926},
    {506, 1.812},
    {171, 0.682},
    {954, 0.449}
};


void Tom8::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 80);
}

void Tom8::Init(std::string slot, float sample_rate, float frequency) {

    this->slot = slot;

    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    oscFilter.Init(sample_rate);
    oscFilter.SetRes(0.5);
    SetParam(PARAM_FREQUENCY, frequency);  // sets oscillator and bpf frequency

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(5);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-10);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, 1.1);

    noise.Init();
    noiseEnv.Init(sample_rate);
    noiseEnv.SetMax(1);
    noiseEnv.SetMin(0);
    noiseEnv.SetCurve(-10);
    noiseEnv.SetTime(ADENV_SEG_ATTACK, 0.050);
    noiseFilter.Init();
    noiseFilter.SetFilterMode(OnePole::FILTER_MODE_LOW_PASS);
    noiseFilter.SetFrequency(0.005);

    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    pitchEnv.SetCurve(-20);
    pitchEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    pitchEnv.SetTime(ADENV_SEG_DECAY, 0.25);
}

float Tom8::Process() {

    // sine osc freq is modulated by pitch env, amp by amp env
    float pitchEnvSignal = pitchEnv.Process();
    float ampEnvSignal = ampEnv.Process();
    osc.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue() + 180 * pitchEnvSignal);
    float oscSignal = osc.Process() * ampEnvSignal;
    oscFilter.SetFreq(parameters[PARAM_FREQUENCY].GetScaledValue());
    oscFilter.Process(oscSignal);
    oscSignal = oscFilter.Band() * 2;

    float noiseEnvSignal = noiseEnv.Process();
    float noiseSignal = noise.Process();   
    noiseSignal = noiseFilter.Process(noiseSignal);   
    noiseSignal = noiseSignal * noiseEnvSignal * 0.5;   

    active = ampEnv.IsRunning() || noiseEnv.IsRunning();
    return (oscSignal + noiseSignal) * velocity; 
}

void Tom8::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        osc.Reset();
        active = true;
        ampEnv.Trigger();
        pitchEnv.Trigger();
        noiseEnv.Trigger();
    }
}

float Tom8::GetParam(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
            case PARAM_AMP_DECAY: 
                return parameters[param].GetScaledValue();
        }
    }

    return 0.0f;
}

std::string Tom8::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                return std::to_string((int)GetParam(param));
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
        }
    }
   return "";
 }

float Tom8::UpdateParam(uint8_t param, float raw) {
    float scaled = raw;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 20, 2000, Parameter::EXPONENTIAL));
                break;
            case PARAM_AMP_DECAY: 
                scaled = parameters[param].Update(raw, Utility::ScaleFloat(raw, 0.01, 5, Parameter::EXPONENTIAL));
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                noiseEnv.SetTime(ADENV_SEG_DECAY, scaled * 1.5);
                break;
        }
    }

    return scaled;    
}

void Tom8::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void Tom8::SetParam(uint8_t param, float scaled) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_FREQUENCY: 
                parameters[param].SetScaledValue(scaled);
                break;
            case PARAM_AMP_DECAY: 
                parameters[param].SetScaledValue(scaled);
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                noiseEnv.SetTime(ADENV_SEG_DECAY, scaled * 1.5);
                break;
        }
    }
}

void Tom8::LoadPreset(u8 preset) {
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
