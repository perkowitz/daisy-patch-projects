#include "Toph.h"

using namespace daisy;
using namespace daisysp;

void Toph::Init(float sampleRate) {
    // set up the params and arrange them in pages
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_SAW].Init("Saw", 0.9, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSE].Init("Pulse", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SUB].Init("Sub", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SAW2].Init("Sw2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSEWIDTH].Init("Width", 0.5, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_PITCH_SENV].Init("SEn1", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PITCH_LFO].Init("Vib", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_LFO_RATE].Init("Rate", 6.66, 0.05, 100, Parameter::EXPONENTIAL, 100);
    params[PARAM_FREQ].Init("Freq", 10000, MIN_FILTER_FREQUENCY, MAX_FILTER_FREQUENCY, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_FILT_SENV2].Init("SEn2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_A].Init("A", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_H].Init("H", 0, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_D].Init("D", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_STEPS].Init("16ths", 16, 0, 128, Parameter::LINEAR, 1);
    params[PARAM_SENV2_A].Init("A", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV2_H].Init("H", 0, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV2_D].Init("D", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV2_STEPS].Init("16ths", 16, 0, 128, Parameter::LINEAR, 1);
    params[PARAM_OUT_12].Init("1-2", 0, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0.8, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 2, Parameter::EXPONENTIAL, 100);

    // assign nullptr to leave a slot blank
    u8 p = 0;
    pages[p++].Init(Name(), "Osc", &params[PARAM_SAW], &params[PARAM_PULSE], &params[PARAM_SUB], &params[PARAM_SAW2]);
    // pages[p++].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], &params[PARAM_PITCH_SENV], nullptr);
    pages[p++].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], &params[PARAM_PITCH_LFO], &params[PARAM_LFO_RATE]);
    pages[p++].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_FENV], &params[PARAM_FILT_SENV2]);
    pages[p++].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[p++].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[p++].Init(Name(), "SyncEnv1", &params[PARAM_SENV_A], &params[PARAM_SENV_H], &params[PARAM_SENV_D], &params[PARAM_SENV_STEPS]);
    pages[p++].Init(Name(), "SyncEnv2", &params[PARAM_SENV2_A], &params[PARAM_SENV2_H], &params[PARAM_SENV2_D], &params[PARAM_SENV2_STEPS]);
    pages[p++].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], nullptr);

    // audio settings -- only set the values that are not set in Process()
    // oscillators
    multiOsc.Init(sampleRate);

    // filter
    vcf.Init(sampleRate);
    vcf.SetFilterMode(LadderFilter::FilterMode::LP24);
    vcf.SetInputDrive(0.5);
    filtEnv.Init(sampleRate);
    filtEnv.SetCurve(1);

    // amp
    ampEnv.Init(sampleRate);
    ampEnv.SetCurve(1);

    syncEnv1.Init(sampleRate);
    syncEnv1.SetCurve(2);

    syncEnv2.Init(sampleRate);
    syncEnv2.SetCurve(2);

    lfo.Init(sampleRate);
    lfo.SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
    lfo.SetFreq(params[PARAM_LFO_RATE].Value());

}

float Toph::Process() {

    syncEnv1.SetStageTime(DahdEnv::STAGE_ATTACK, params[PARAM_SENV_A].Value());
    syncEnv1.SetStageTime(DahdEnv::STAGE_HOLD, params[PARAM_SENV_H].Value());
    syncEnv1.SetStageTime(DahdEnv::STAGE_DECAY, params[PARAM_SENV_D].Value());
    syncEnv1.SetSyncSteps((int)params[PARAM_SENV_STEPS].Value());
    float syncEnv1Signal = syncEnv1.Process();

    syncEnv2.SetStageTime(DahdEnv::STAGE_ATTACK, params[PARAM_SENV2_A].Value());
    syncEnv2.SetStageTime(DahdEnv::STAGE_HOLD, params[PARAM_SENV2_H].Value());
    syncEnv2.SetStageTime(DahdEnv::STAGE_DECAY, params[PARAM_SENV2_D].Value());
    syncEnv2.SetSyncSteps((int)params[PARAM_SENV2_STEPS].Value());
    float syncEnv2Signal = syncEnv2.Process();

    lfo.SetFreq(params[PARAM_LFO_RATE].Value());
    float lfoSignal = lfo.Process();

    // oscillators
    // multiOsc.SetFreq(mtof(note) * (1 + (params[PARAM_PITCH_SENV].Value() * syncEnvSignal)));
    multiOsc.SetFreq(mtof(note) + 20 * params[PARAM_PITCH_LFO].Value() * lfoSignal);
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
    float freq = params[PARAM_FREQ].Value();
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_FENV].Value() * params[PARAM_FENV].Value() * filtEnv.Process();
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_FILT_SENV2].Value() * params[PARAM_FILT_SENV2].Value() * syncEnv2Signal;
    vcf.SetFreq(freq);
    vcf.SetRes(params[PARAM_RES].Value());
    signal = vcf.Process(signal);

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
    this->note = note;

    if (note >= 0 && note < 128) {
        multiOsc.SetFreq(mtof(note));
        multiOsc.Reset();
        lfo.Reset();
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
    syncEnv1.Clock(measure, step, tick);
    syncEnv2.Clock(measure, step, tick);
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
