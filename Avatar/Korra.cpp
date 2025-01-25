#include "Korra.h"

using namespace daisy;
using namespace daisysp;


void Korra::Init(float sampleRate) {
    Init(sampleRate, VOICE_COUNT);
}

void Korra::Init(float sampleRate, u8 voiceLimit) {
    // set up the params and arrange them in pages
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_SAW].Init("Saw", 0.8, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSE].Init("Pulse", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SUB].Init("Sub", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_SAW2].Init("Sw2", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_PULSEWIDTH].Init("Width", 0.5, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FREQ].Init("Freq", 1000, MIN_FILTER_FREQUENCY, MAX_FILTER_FREQUENCY, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_F_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_F_SENV].Init("SEnv", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_F_DRIFT].Init("Drft", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_FRES_DRIFT].Init("D>Res", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_HPF].Init("HPF", 0, 0, 0.2, Parameter::CUBE, 500);
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_DELAY].Init("Del", 0, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_A].Init("A", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_DECAY].Init("D", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_STEPS].Init("16ths", 16, 0, 128, Parameter::LINEAR, 1);
    params[PARAM_KLOK].Init("Klok", 0, 0, KORRA_MAX_KLOK, Parameter::LINEAR, 1);
    params[PARAM_DRIFT_RATE].Init("Rate", 8, 0, KORRA_MAX_DRIFT_RATE, Parameter::LINEAR, 1);
    params[PARAM_DRIFT_LOOP].Init("Loop", 8, 1, DRIFT_STEPS, Parameter::LINEAR, 1);
    params[PARAM_FOLD].Init("Fold", 0, 0, 25, Parameter::EXPONENTIAL, 4);
    params[PARAM_WRAP].Init("Wrap", 0, 0, 25, Parameter::EXPONENTIAL, 4);
    params[PARAM_SQUEEZE].Init("Sqz", 0, 0, 25, Parameter::EXPONENTIAL, 4);
    params[PARAM_DR2FOLD].Init("Dr>Fo", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_12].Init("1-2", 0.8, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 2, Parameter::EXPONENTIAL, 100);

    // assign nullptr to leave a slot blank
    u8 p = 0;
    pages[p++].Init(Name(), "Osc", &params[PARAM_SAW], &params[PARAM_PULSE], &params[PARAM_SUB], &params[PARAM_SAW2]);
    pages[p++].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], nullptr, nullptr);
    pages[p++].Init(Name(), "Osc", &params[PARAM_FOLD], &params[PARAM_WRAP], &params[PARAM_SQUEEZE], &params[PARAM_DR2FOLD]);
    pages[p++].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_HPF], &params[PARAM_KLOK]);
    pages[p++].Init(Name(), "Mod>Filt", &params[PARAM_F_FENV], &params[PARAM_F_SENV], &params[PARAM_F_DRIFT], &params[PARAM_FRES_DRIFT]);
    pages[p++].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[p++].Init(Name(), "SyncEnv", &params[PARAM_SENV_DELAY], &params[PARAM_SENV_A], &params[PARAM_SENV_DECAY], &params[PARAM_SENV_STEPS]);
    pages[p++].Init(Name(), "Drift", &params[PARAM_DRIFT_RATE], &params[PARAM_DRIFT_LOOP], nullptr, nullptr);
    pages[p++].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[p++].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], nullptr);

    if (voiceLimit > 0 && voiceLimit < VOICE_COUNT) {
        this->voiceLimit = voiceLimit;
    }
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        voices[voice].multiOsc.Init(sampleRate);
        voices[voice].ampEnv.Init(sampleRate);
        voices[voice].ampEnv.SetCurve(1);
    }

    vcf.Init(sampleRate);
    vcf.SetFilterMode(LadderFilter::FilterMode::LP24);
    vcf.SetInputDrive(0.5);
    filtEnv.Init(sampleRate);
    filtEnv.SetCurve(1);
    hpf.Init();
    hpf.SetFilterMode(OnePole::FILTER_MODE_HIGH_PASS);
    hpf.SetFrequency(0);
    
    syncEnv.Init(sampleRate);
    syncEnv.SetCurve(2);

    drift.Init();
    drift.SetLoopLength(8);
    drift.SetRate(8);
}

void Korra::ProcessChanges() {

    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // oscillator
        voices[voice].multiOsc.SetPulsewidth(params[PARAM_PULSEWIDTH].Value());

        // amp
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_A].Value());
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_D].Value());
        voices[voice].ampEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_R].Value());
        voices[voice].ampEnv.SetSustainLevel(params[PARAM_S].Value());
    }

    // filter
    filtEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, params[PARAM_FA].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_DECAY, params[PARAM_FD].Value());
    filtEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, params[PARAM_FR].Value());
    filtEnv.SetSustainLevel(params[PARAM_FS].Value());

    syncEnv.SetStageTime(DahdEnv::STAGE_ATTACK, params[PARAM_SENV_A].Value());
    syncEnv.SetStageTime(DahdEnv::STAGE_HOLD, 0);
    syncEnv.SetStageTime(DahdEnv::STAGE_DELAY, params[PARAM_SENV_DELAY].Value());
    syncEnv.SetStageTime(DahdEnv::STAGE_DECAY, params[PARAM_SENV_DECAY].Value());
    syncEnv.SetSyncSteps((int)params[PARAM_SENV_STEPS].Value());
    hpf.SetFrequency(params[PARAM_HPF].Value());

    drift.SetLoopLength(params[PARAM_DRIFT_LOOP].Value());
    drift.SetRate(params[PARAM_DRIFT_RATE].Value());
}

float Korra::Process() {

    klokCount = (klokCount + 1) % KORRA_KLOK_COUNT_LIMIT;
    u8 k = 0;
    switch (currentKlok) {
        case 0: 
            klokCount = 0;
            break;
        case 1:
            k = klokCount % 2;
            break;
        case 2:
            k = klokCount % 4;
            break;
        case 3:
            k = klokCount % 8;
            break;
    }
    if (k != 0) {
        return 0;
    }

    float left = 0;
    float right = 0;

    float syncEnvSignal = syncEnv.Process();

    float signal = 0;
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // oscillators
        voices[voice].multiOsc.Process();
        float oscSignal = params[PARAM_SAW].Value() * voices[voice].multiOsc.Saw();
        oscSignal += params[PARAM_PULSE].Value() * voices[voice].multiOsc.Pulse();
        oscSignal += params[PARAM_SUB].Value() * voices[voice].multiOsc.Sub();
        oscSignal += params[PARAM_SAW2].Value() * voices[voice].multiOsc.Saw2();
        // signal += voices[voice].velocity * oscSignal * voices[voice].ampEnv.Process();
        oscSignal = Folding::Fold(oscSignal, params[PARAM_FOLD].Value() + params[PARAM_DR2FOLD].Value() * 25 * drift.Signal(2));
        oscSignal = Folding::Wrap(oscSignal, params[PARAM_WRAP].Value());
        oscSignal = Folding::Squeeze(oscSignal, params[PARAM_SQUEEZE].Value());
        signal += oscSignal * voices[voice].ampEnv.Process();
        voices[voice].isPlaying = voices[voice].ampEnv.IsRunning();
    }

    // filter
    float freq = params[PARAM_FREQ].Value();
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_F_FENV].Value() * params[PARAM_F_FENV].Value() * filtEnv.Process();
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_F_SENV].Value() * params[PARAM_F_SENV].Value() * syncEnvSignal;
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_F_DRIFT].Value() * drift.Signal();
    vcf.SetFreq(freq);
    vcf.SetRes(params[PARAM_RES].Value() + params[PARAM_FRES_DRIFT].Value() * drift.Signal(1));
    signal = vcf.Process(signal);

    // left = hpf.Process(signal);
    // leftSignal = left / voiceLimit;
    leftSignal = hpf.Process(signal);
    rightSignal = leftSignal;

    return leftSignal;
}

float Korra::GetOutput(u8 channel) {
    return leftSignal;
    switch (channel) {
        case 0: return leftSignal * params[PARAM_OUT_12].Value();
        case 1: return rightSignal * params[PARAM_OUT_12].Value();
        case 2: return leftSignal * params[PARAM_OUT_3].Value();
        case 3: return rightSignal * params[PARAM_OUT_4].Value();
    }
    return 0;
}

void Korra::NoteOn(u8 note, float velocity) {
    float v = Utility::Limit(velocity);
    if (v == 0) {
        NoteOff(note);
        return;
    }

    // look for a voice that isn't playing (incl env release)
    s8 assignedVoice = -1;
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        if (!voices[voice].isPlaying && !voices[voice].gateIsOn) {
            assignedVoice = voice;
        }
    }

    // if nothing found, look for a voice that's just gated off
    if (assignedVoice == -1) {
        for (u8 vox = 0; vox < voiceLimit; vox++) {
            if (!voices[vox].gateIsOn) {
                assignedVoice = vox;
            }
        }
    }

    // otherwise just take the next voice
    if (assignedVoice == -1) {
        assignedVoice = nextVoice;
        nextVoice = (nextVoice + 1) % voiceLimit;
    }

    // if the voice is being used, take it over but do not retrigger (i.e. leave gates alone)
    // if (voices[assignedVoice].isPlaying) {
    //     // should call noteoff, but have to tell it which voice
    //     voices[assignedVoice].isPlaying = false;
    // }

    // update klok here instead of ProcessUpdates so it changes on next noteOn
    // (though of course klok rate affects all voices)
    currentKlok = (int)params[PARAM_KLOK].Value();

    voices[assignedVoice].note = note;
    u8 adjustedNote = note + (int)params[PARAM_OCTAVE].Value() * 12 + currentKlok * 12;
    if (adjustedNote >= 0 && adjustedNote < 128) {
        voices[assignedVoice].multiOsc.SetFreq(mtof(adjustedNote));
        // voices[assignedVoice].multiOsc.Reset();
        voices[assignedVoice].velocity = v;

        // if the voice wasn't already playing, add a gate; if this is first gate, trigger envelopes
        if (!voices[assignedVoice].gateIsOn) {
            voices[assignedVoice].gateIsOn = true;
            voices[assignedVoice].ampEnv.GateOn();
            gates++;
            if (gates == 1) {
                filtEnv.GateOn();
                drift.Trigger();
            }
        }
    }
}

void Korra::NoteOff(u8 note) {
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        if (note == voices[voice].note) {
            voices[voice].gateIsOn = false;
            voices[voice].ampEnv.GateOff();
        }
    }
    gates--;
    if (gates == 0) {
        filtEnv.GateOff();
    }
}

void Korra::Clock(u8 measure, u8 step, u8 tick) {
    syncEnv.Clock(measure, step, tick);
}

ParamPage *Korra::GetParamPage(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

void Korra::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        paramSets[page].ResetParams();
    }
}

Param *Korra::GetParam(u8 index) { 
    if (index < PARAM_COUNT) {
        return &params[index];
    }
    return nullptr;
}
