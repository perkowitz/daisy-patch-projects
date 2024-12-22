#include "Toph.h"

using namespace daisy;
using namespace daisysp;

void Toph::Init(float sampleRate) {
    // set up the params and arrange them in pages
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_SAW].Init("Saw", 1, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSE].Init("Pulse", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SUB].Init("Sub", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SAW2].Init("Sw2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSEWIDTH].Init("Width", 0.5, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FREQ].Init("Freq", 1000, 0, MAX_FREQ, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_OUT_12].Init("1-2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0.8, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 1, Parameter::EXPONENTIAL, 100);

    // assign nullptr to leave a slot blank
    pages[0].Init(Name(), "Osc", &params[PARAM_SAW], &params[PARAM_PULSE], &params[PARAM_SUB], &params[PARAM_SAW2]);
    pages[1].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], nullptr, nullptr);
    pages[2].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_FENV], nullptr);
    pages[3].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[4].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[5].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], nullptr);

    // audio settings -- only set the values that are not set in Process()
    // oscillators
    multiOsc.Init(sampleRate);

    // filter
    svf.Init(sampleRate);
    svf.SetDrive(0.1);
    svf2.Init(sampleRate);
    svf2.SetDrive(0.1);
    filtEnv.Init(sampleRate);
    filtEnv.SetCurve(1);

    // amp
    ampEnv.Init(sampleRate);
    ampEnv.SetCurve(1);
}

float Toph::Process() {

    // oscillators
    multiOsc.SetPulsewidth(params[PARAM_PULSEWIDTH].Value());
    multiOsc.Process();
    float signal = params[PARAM_SAW].Value() * multiOsc.Saw();
    signal += params[PARAM_PULSE].Value() * multiOsc.Pulse();
    signal += params[PARAM_SUB].Value() * multiOsc.Sub();
    signal += params[PARAM_SAW2].Value() * multiOsc.Saw2();

    // filter
    filtEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_FA].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_FD].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_FR].Value());
    filtEnv.SetSustainLevel(params[PARAM_FS].Value());
    // svf.SetFreq(params[PARAM_FREQ].Value());
    svf.SetFreq(params[PARAM_FREQ].Value() + MAX_FREQ * params[PARAM_FENV].Value() * params[PARAM_FENV].Value() * filtEnv.Process());
    svf.SetRes(params[PARAM_RES].Value());
    svf.Process(signal);
    // signal = svf.Low();
    svf2.SetFreq(params[PARAM_FREQ].Value() + MAX_FREQ * params[PARAM_FENV].Value() * filtEnv.Process());
    svf2.SetRes(params[PARAM_RES].Value());
    svf2.Process(svf.Low());
    signal = svf2.Low();

    // amp
    ampEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_A].Value());
    ampEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_D].Value());
    ampEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_R].Value());
    ampEnv.SetSustainLevel(params[PARAM_S].Value());
    leftSignal = velocity * signal * ampEnv.Process();
    rightSignal = leftSignal;

    active = ampEnv.IsRunning();
    return leftSignal;
}

float Toph::GetOutput(u8 channel) {
    switch (channel) {
        case 0: return leftSignal * params[PARAM_OUT_12].Value();
        case 1: return rightSignal * params[PARAM_OUT_12].Value();
        case 2: return leftSignal * params[PARAM_OUT_3].Value();
        case 3: return rightSignal * params[PARAM_OUT_4].Value();
    }
    return 0;
}

void Toph::NoteOn(u8 note, float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity == 0) {
        NoteOff(note);
        return;
    }

    note += (int)params[PARAM_OCTAVE].Value() * 12;

    if (note >= 0 && note < 128) {
        multiOsc.SetFreq(mtof(note));
        multiOsc.Reset();
        ampEnv.GateOn();
        filtEnv.GateOn();
        // if (retrigger || activeGates == 0) {
        //     ampEnv.Retrigger(false);
        // }
        activeGates++;
        active = true;
    }
}

void Toph::NoteOff(u8 note) {
    // if (activeGates > 0) {
    //     activeGates--;
    // }
    ampEnv.GateOff();
    filtEnv.GateOff();
}

void Toph::Clock(u8 measure, u8 step, u8 tick) {
    
}

ParamPage *Toph::GetParamPage(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

void Toph::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        paramSets[page].ResetParams();
    }
}

Param *Toph::GetParam(u8 index) { 
    if (index < PARAM_COUNT) {
        return &params[index];
    }
    return nullptr;
}
