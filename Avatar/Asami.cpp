#include "Asami.h"
#include <ctime>

using namespace daisy;
using namespace daisysp;


void Asami::Init(float sampleRate) {
    Init(sampleRate, VOICE_COUNT);
}

void Asami::Init(float sampleRate, u8 voiceLimit) {
    // osc1
    params[PARAM_OCTAVE].Init("Oct", 0, -3, 3, Parameter::LINEAR, 1);
    params[PARAM_SAMPLE_START].Init("Start", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_SAW].Init("Saw", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    // filter
    params[PARAM_FREQ].Init("Freq", MAX_FREQ, 0, MAX_FREQ, Parameter::EXPONENTIAL, 1);
    params[PARAM_RES].Init("Reso", 0, 0, 1, Parameter::LINEAR, 100);
    params[PARAM_FENV].Init("Env", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_F_SENV].Init("SEnv", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_HPF].Init("HPF", 0, 0, 0.2, Parameter::CUBE, 500);
    params[PARAM_FA].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FD].Init("D", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_FS].Init("S", 0, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_FR].Init("R", 0.2, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_KLOK].Init("Klok", 0, 0, ASAMI_MAX_KLOK, Parameter::LINEAR, 1);
    // modulators
    params[PARAM_A].Init("A", 0, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_D].Init("D", 0.02, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_S].Init("S", 0.8, 0.0, 1.0, Parameter::LINEAR, 100);
    params[PARAM_R].Init("R", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TA].Init("A", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TH].Init("H", 0, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_TD].Init("D", 0.5, 0.0, 10.0, Parameter::EXPONENTIAL, 1000);
    params[PARAM_SENV_STEPS].Init("16ths", 16, 0, 128, Parameter::LINEAR, 1);
    // output
    params[PARAM_OUT_12].Init("1-2", 0.8, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 1, Parameter::EXPONENTIAL, 100);

    // assign nullptr to leave a slot blank
    pages[0].Init(Name(), "Osc1", &params[PARAM_OCTAVE], &params[PARAM_SAMPLE_START], &params[PARAM_SAW], nullptr);
    pages[1].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_FENV], &params[PARAM_F_SENV]);
    pages[2].Init(Name(), "Mix", &params[PARAM_HPF], &params[PARAM_KLOK], nullptr, nullptr);
    pages[3].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[4].Init(Name(), "SyncEnv", &params[PARAM_TA], &params[PARAM_TH], &params[PARAM_TD], &params[PARAM_SENV_STEPS]);
    pages[5].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[6].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], nullptr);

    srand (static_cast <unsigned> (time(0)));
    for (u32 s = 0; s < ASAMI_BUFFER_SIZE; s++) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX + 1);
        // waveSamples[s] = r * 2 - 1;
        float sample = (float)s / (float)ASAMI_BUFFER_SIZE;
        waveSamples[s] = (sample + r) * 2 - 1;
    }

    if (voiceLimit > 0 && voiceLimit < VOICE_COUNT) {
        this->voiceLimit = voiceLimit;
    }
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        voices[voice].waveSyncOsc.Init(sampleRate);
        voices[voice].waveSyncOsc.SetBuffer((float*)&waveSamples, ASAMI_BUFFER_SIZE);
        voices[voice].ampEnv.Init(sampleRate);
        voices[voice].ampEnv.SetCurve(1);
    }

    svf.Init(sampleRate);
    svf.SetDrive(0);
    filtEnv.Init(sampleRate);
    filtEnv.SetCurve(1);
    hpf.Init();
    hpf.SetFilterMode(OnePole::FILTER_MODE_HIGH_PASS);
    hpf.SetFrequency(0);
    
    syncEnv.Init(sampleRate);
    syncEnv.SetCurve(2);
}

void Asami::ProcessChanges() {

    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // osc1
        voices[voice].waveSyncOsc.SetStartSample((u32)(params[PARAM_SAMPLE_START].Value() * ASAMI_BUFFER_SIZE));

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

    syncEnv.SetStageTime(DahdEnv::STAGE_ATTACK, params[PARAM_TA].Value());
    syncEnv.SetStageTime(DahdEnv::STAGE_HOLD, params[PARAM_TH].Value());
    syncEnv.SetStageTime(DahdEnv::STAGE_DECAY, params[PARAM_TD].Value());
    syncEnv.SetSyncSteps((int)params[PARAM_SENV_STEPS].Value());
    hpf.SetFrequency(params[PARAM_HPF].Value());
}

float Asami::Process() {

    klokCount = (klokCount + 1) % ASAMI_KLOK_COUNT_LIMIT;
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

    float trigEnvSignal = syncEnv.Process();

    float signal = 0;
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // osc1
        float osc1Signal = voices[voice].waveSyncOsc.Process();
        osc1Signal += params[PARAM_SAW].Value() * voices[voice].waveSyncOsc.Saw();
        // mix
        float envSignal = voices[voice].ampEnv.Process();
        signal += osc1Signal * envSignal;
        voices[voice].isPlaying = voices[voice].ampEnv.IsRunning();
    }

    // filter
    float freq = params[PARAM_FREQ].Value();
    freq += MAX_FREQ * params[PARAM_FENV].Value() * params[PARAM_FENV].Value() * filtEnv.Process();
    freq += MAX_FREQ * params[PARAM_F_SENV].Value() * params[PARAM_F_SENV].Value() * trigEnvSignal;
    svf.SetFreq(freq);
    svf.SetRes(params[PARAM_RES].Value());
    svf.Process(signal);
    signal = svf.Low();

    left = hpf.Process(signal);
    leftSignal = left / voiceLimit;
    rightSignal = leftSignal;

    return leftSignal;
}

float Asami::GetOutput(u8 channel) {
    return leftSignal;
    switch (channel) {
        case 0: return leftSignal * params[PARAM_OUT_12].Value();
        case 1: return rightSignal * params[PARAM_OUT_12].Value();
        case 2: return leftSignal * params[PARAM_OUT_3].Value();
        case 3: return rightSignal * params[PARAM_OUT_4].Value();
    }
    return 0;
}

void Asami::NoteOn(u8 note, float velocity) {
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

    // update klok here instead of ProcessUpdates so it changes on next noteOn
    // (though of course klok rate affects all voices)
    currentKlok = (int)params[PARAM_KLOK].Value();

    voices[assignedVoice].note = note;
    u8 adjustedNote1 = note + (int)params[PARAM_OCTAVE].Value() * 12 + currentKlok * 12;
    if (adjustedNote1 >= 0 && adjustedNote1 < 128) {
        voices[assignedVoice].waveSyncOsc.SetFreq(mtof(adjustedNote1));
        voices[assignedVoice].velocity = v;

        // if the voice wasn't already playing, add a gate; if this is first gate, trigger envelopes
        if (!voices[assignedVoice].gateIsOn) {
            voices[assignedVoice].gateIsOn = true;
            voices[assignedVoice].ampEnv.GateOn();
            gates++;
            if (gates == 1) {
                filtEnv.GateOn();
            }
        }
    }
}

void Asami::NoteOff(u8 note) {
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

void Asami::Clock(u8 measure, u8 step, u8 tick) {
    syncEnv.Clock(measure, step, tick);
}

ParamPage *Asami::GetParamPage(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

void Asami::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        paramSets[page].ResetParams();
    }
}

Param *Asami::GetParam(u8 index) { 
    if (index < PARAM_COUNT) {
        return &params[index];
    }
    return nullptr;
}
