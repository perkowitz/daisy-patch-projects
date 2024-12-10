#include "Simple.h"

using namespace daisy;
using namespace daisysp;

void Simple::Init(float sampleRate) {
    // set up the params and arrange them in pages
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_FREQ].Init("Freq", 1000, 0, MAX_FREQ, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    // assign nullptr to leave a slot blank
    pages[0].Init(ShortName(), "Osc", &params[PARAM_OCTAVE], nullptr, nullptr, nullptr);
    pages[1].Init(ShortName(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_FENV], nullptr);
    pages[2].Init(ShortName(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[3].Init(ShortName(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);

    // audio settings -- only set the values that are not set in Process()
    // oscillator
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc.SetAmp(1);

    // filter
    svf.Init(sampleRate);
    svf.SetDrive(.002);
    filtEnv.Init(sampleRate);

    // amp
    ampEnv.Init(sampleRate);
}

float Simple::Process() {

    // oscillator
    float signal = osc.Process();

    // filter
    filtEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_FA].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_FD].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_FR].Value());
    filtEnv.SetSustainLevel(params[PARAM_FS].Value());
    // svf.SetFreq(params[PARAM_FREQ].Value());
    svf.SetFreq(params[PARAM_FREQ].Value() + MAX_FREQ * params[PARAM_FENV].Value() * filtEnv.Process());
    svf.SetRes(params[PARAM_RES].Value());
    svf.Process(signal);
    signal = svf.Low();

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

float Simple::GetOutput(u8 channel) {
    return channel == 0 ? leftSignal : rightSignal;
}

void Simple::NoteOn(u8 note, float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity == 0) {
        NoteOff(note);
        return;
    }

    note +=  (int)params[PARAM_OCTAVE].Value() * 12;

    if (note >= 0 && note < 128) {
        osc.SetFreq(mtof(note));
        osc.Reset();
        ampEnv.GateOn();
        filtEnv.GateOn();
        // if (retrigger || activeGates == 0) {
        //     ampEnv.Retrigger(false);
        // }
        activeGates++;
        active = true;
    }
}

void Simple::NoteOff(u8 note) {
    // if (activeGates > 0) {
    //     activeGates--;
    // }
    ampEnv.GateOff();
    filtEnv.GateOff();
}

ParamPage *Simple::GetParamPage(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

void Simple::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        paramSets[page].ResetParams();
    }
}
