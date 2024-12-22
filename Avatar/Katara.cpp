#include "Katara.h"

using namespace daisy;
using namespace daisysp;


void Katara::Init(float sampleRate) {
    Init(sampleRate, VOICE_COUNT);
}

void Katara::Init(float sampleRate, u8 voiceLimit) {
    // set up the params and arrange them in pages
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_SAW].Init("Saw", 0.8, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSE].Init("Pulse", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SUB].Init("Sub", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SAW2].Init("Sw2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSEWIDTH].Init("Width", 0.5, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FREQ].Init("Freq", 1000, 0, MAX_FREQ, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_F_SENV].Init("SEnv", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_HPF].Init("HPF", 0, 0, 0.2, Parameter::CUBE, 500);
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TA].Init("A", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TH].Init("H", 0, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TD].Init("D", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_STEPS].Init("16ths", 16, 0, 128, Parameter::LINEAR, 1);
    params[PARAM_OUT_12].Init("1-2", 0.8, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 1, Parameter::EXPONENTIAL, 100);

    // assign nullptr to leave a slot blank
    pages[0].Init(Name(), "Osc", &params[PARAM_SAW], &params[PARAM_PULSE], &params[PARAM_SUB], &params[PARAM_SAW2]);
    pages[1].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], nullptr, nullptr);
    pages[2].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_FENV], &params[PARAM_F_SENV]);
    pages[3].Init(Name(), "Filt", &params[PARAM_HPF], nullptr, nullptr, nullptr);
    pages[4].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[5].Init(Name(), "SyncEnv", &params[PARAM_TA], &params[PARAM_TH], &params[PARAM_TD], &params[PARAM_SENV_STEPS]);
    pages[6].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[7].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], nullptr);

    if (voiceLimit > 0 && voiceLimit < VOICE_COUNT) {
        this->voiceLimit = voiceLimit;
    }
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // audio settings -- only set the values that are not set in Process()
        // oscillators
        voices[voice].multiOsc.Init(sampleRate);

        // filter
        voices[voice].svf.Init(sampleRate);
        voices[voice].svf.SetDrive(0);
        voices[voice].filtEnv.Init(sampleRate);
        voices[voice].filtEnv.SetCurve(1);

        // amp
        voices[voice].ampEnv.Init(sampleRate);
        voices[voice].ampEnv.SetCurve(1);
    }

    hpf.Init();
    hpf.SetFilterMode(OnePole::FILTER_MODE_HIGH_PASS);
    hpf.SetFrequency(0);
    
    syncEnv.Init(sampleRate);
    syncEnv.SetCurve(2);
}

void Katara::ProcessChanges() {

    syncEnv.SetStageTime(AhdEnv::STAGE_ATTACK, params[PARAM_TA].Value());
    syncEnv.SetStageTime(AhdEnv::STAGE_HOLD, params[PARAM_TH].Value());
    syncEnv.SetStageTime(AhdEnv::STAGE_DECAY, params[PARAM_TD].Value());
    syncEnv.SetSyncSteps((int)params[PARAM_SENV_STEPS].Value());
    hpf.SetFrequency(params[PARAM_HPF].Value());

    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // oscillator
        voices[voice].multiOsc.SetPulsewidth(params[PARAM_PULSEWIDTH].Value());

        // filter
        voices[voice].filtEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_FA].Value());
        voices[voice].filtEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_FD].Value());
        voices[voice].filtEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_FR].Value());
        voices[voice].filtEnv.SetSustainLevel(params[PARAM_FS].Value());

        // amp
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_A].Value());
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_D].Value());
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_R].Value());
        voices[voice].ampEnv.SetSustainLevel(params[PARAM_S].Value());
    }
}

float Katara::Process() {

    float left = 0;
    float right = 0;

    float trigEnvSignal = syncEnv.Process();

    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // oscillators
        voices[voice].multiOsc.Process();
        float signal = params[PARAM_SAW].Value() * voices[voice].multiOsc.Saw();
        signal += params[PARAM_PULSE].Value() * voices[voice].multiOsc.Pulse();
        signal += params[PARAM_SUB].Value() * voices[voice].multiOsc.Sub();
        signal += params[PARAM_SAW2].Value() * voices[voice].multiOsc.Saw2();

        // filter
        float freq = params[PARAM_FREQ].Value();
        freq += MAX_FREQ * params[PARAM_FENV].Value() * params[PARAM_FENV].Value() * voices[voice].filtEnv.Process();
        freq += MAX_FREQ * params[PARAM_F_SENV].Value() * params[PARAM_F_SENV].Value() * trigEnvSignal;
        voices[voice].svf.SetFreq(freq);
        voices[voice].svf.SetRes(params[PARAM_RES].Value());
        voices[voice].svf.Process(signal);
        signal = voices[voice].svf.Low();

        // amp
        // left += voices[voice].velocity * signal * voices[voice].ampEnv.Process();
        left += signal * voices[voice].ampEnv.Process();
        right += voices[voice].velocity * signal * voices[voice].ampEnv.Process();
    }
    left = hpf.Process(left);

    leftSignal = MIX_SCALE * left / voiceLimit;
    // rightSignal = MIX_SCALE * right / voiceLimit;
    rightSignal = leftSignal;

    return leftSignal;
}

float Katara::GetOutput(u8 channel) {
    switch (channel) {
        case 0: return leftSignal * params[PARAM_OUT_12].Value();
        case 1: return rightSignal * params[PARAM_OUT_12].Value();
        case 2: return leftSignal * params[PARAM_OUT_3].Value();
        case 3: return rightSignal * params[PARAM_OUT_4].Value();
    }
    return 0;
}

void Katara::NoteOn(u8 note, float velocity) {
    float v = Utility::Limit(velocity);
    if (v == 0) {
        NoteOff(note);
        return;
    }

    s8 assignedVoice = -1;
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        if (!voices[voice].gateOn) {
            assignedVoice = voice;
        }
    }

    if (assignedVoice == -1) {
        assignedVoice = nextVoice;
        nextVoice = (nextVoice + 1) % voiceLimit;
    }
    if (voices[assignedVoice].gateOn) {
        // should call noteoff, but have to tell it which voice
        voices[assignedVoice].ampEnv.GateOff();
        voices[assignedVoice].filtEnv.GateOff();
        voices[assignedVoice].gateOn = false;
    }

    voices[assignedVoice].note = note;
    u8 adjustedNote = note + (int)params[PARAM_OCTAVE].Value() * 12;
    if (adjustedNote >= 0 && adjustedNote < 128) {
        voices[assignedVoice].multiOsc.SetFreq(mtof(adjustedNote));
        voices[assignedVoice].multiOsc.Reset();
        voices[assignedVoice].ampEnv.GateOn();
        voices[assignedVoice].filtEnv.GateOn();
        voices[assignedVoice].gateOn = true;
        voices[assignedVoice].velocity = v;
    }
}

void Katara::NoteOff(u8 note) {
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        if (note == voices[voice].note) {
            voices[voice].ampEnv.GateOff();
            voices[voice].filtEnv.GateOff();
            voices[voice].gateOn = false;
        }
    }
}

void Katara::Clock(u8 measure, u8 step, u8 tick) {
    syncEnv.Clock(measure, step, tick);
}

ParamPage *Katara::GetParamPage(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

void Katara::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        paramSets[page].ResetParams();
    }
}

Param *Katara::GetParam(u8 index) { 
    if (index < PARAM_COUNT) {
        return &params[index];
    }
    return nullptr;
}
