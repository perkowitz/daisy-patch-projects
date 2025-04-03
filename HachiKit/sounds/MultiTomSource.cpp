#include "MultiTomSource.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;


void MultiTomSource::Init(std::string slot, float sample_rate) {
    Init(slot, sample_rate, 80, NULL);
}

void MultiTomSource::Init(std::string slot, float sample_rate, float decay, ClickSource *clickSource) {

    this->slot = slot;
    this->clickSource = clickSource;

    osc.Init(sample_rate);
    currentBaseFrequency = 500;
    osc.SetFreq(currentBaseFrequency);
    osc.SetWaveform(Oscillator::WAVE_TRI);

    ampEnv.Init(sample_rate);
    ampEnv.SetMax(1);
    ampEnv.SetMin(0);
    ampEnv.SetCurve(-10);
    ampEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    SetParam(PARAM_AMP_DECAY, 1.1);
    // SetParam(PARAM_PITCH_MOD, 60);

    pitchEnv.Init(sample_rate);
    pitchEnv.SetMax(1);
    pitchEnv.SetMin(0);
    pitchEnv.SetCurve(-20);
    pitchEnv.SetTime(ADENV_SEG_ATTACK, 0.001);
    pitchEnv.SetTime(ADENV_SEG_DECAY, 0.25);
}

float MultiTomSource::Process() {

    float clickSignal = clickSource == NULL ? 0.0f : clickSource->Signal();
    
    // sine osc freq is modulated by pitch env, amp by amp env
    float pitchEnvSignal = pitchEnv.Process();
    ampEnvSignal = ampEnv.Process();
    osc.SetFreq(currentBaseFrequency + 60 * pitchEnvSignal);
    float oscSignal = osc.Process() * ampEnvSignal;

    active = ampEnv.IsRunning();
    signal = (clickSignal + oscSignal) * velocity * currentGain; 
    return signal;
}

void MultiTomSource::Trigger(float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        currentGain = 1.0f;
        active = true;
        // clickSource->Trigger(velocity);
        ampEnv.Trigger();
        // pitchEnv.Trigger();
        osc.Reset();
    }
}

void MultiTomSource::Trigger(float velocity, u8 index, float frequency) {
    if (ampEnvSignal <= retriggerThreshold) {
        currentBaseFrequency = frequency;
        currentIndex = index;
        osc.SetFreq(frequency);
        Trigger(velocity);
        return;
    }

    this->velocity = Utility::Limit(velocity);
    if (this->velocity > 0) {
        currentGain *= retriggerGainScale;
        if (currentIndex != index) {
            currentBaseFrequency = std::max(currentBaseFrequency, frequency) + std::abs(currentBaseFrequency - frequency);
        } else {
            currentBaseFrequency *= retriggerFreqScale;
        }
        osc.SetFreq(currentBaseFrequency);
        currentIndex = index;
        ampEnv.Trigger();
        osc.Reset();
    }
}

float MultiTomSource::GetParam(uint8_t param) {
    return param < PARAM_COUNT ? parameters[param].GetScaledValue() : 0.0f;
}

std::string MultiTomSource::GetParamString(uint8_t param) {
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_AMP_DECAY: 
                return std::to_string((int)(GetParam(param) * 1000));// + "ms";
        }
    }
   return "";
 }

float MultiTomSource::UpdateParam(uint8_t param, float raw) {
    return SetParam(param, raw, true); 
}

void MultiTomSource::ResetParams() {
    for (u8 param = 0; param < PARAM_COUNT; param++) {
        parameters[param].Reset();
    }
}

void MultiTomSource::SetParam(uint8_t param, float scaled) {
    SetParam(param, scaled, false);
}

float MultiTomSource::SetParam(uint8_t param, float value, bool isRaw) {
    float scaled = value;
    if (param < PARAM_COUNT) {
        switch (param) {
            case PARAM_AMP_DECAY:
                if (isRaw) {
                    scaled = parameters[param].Update(value, Utility::ScaleFloat(value, 0.001, 10, Parameter::EXPONENTIAL));
                } else {
                    parameters[param].SetScaledValue(value);
                }
                ampEnv.SetTime(ADENV_SEG_DECAY, scaled);
                return scaled;
        }
    }

    return 0.0f;
}

void MultiTomSource::LoadPreset(u8 preset) {

}
