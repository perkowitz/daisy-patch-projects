#include "Korra.h"

using namespace daisy;
using namespace daisysp;


u8 Korra::ctrlParams[MIDI_CC_COUNT] = { PARAM_SAW, PARAM_PULSE, PARAM_SUB, PARAM_SAW2, PARAM_FREQ, PARAM_RES, PARAM_F_FENV, PARAM_FD  };

float Korra::presets[][Korra::PARAM_COUNT] = {        // params are in order of PARAM constant in .h
    {
        1, 900, 0.00,                      // oct, freq, res
        0.000, 0.200, 0.80, 0.300,          // a, d, s, r
        0.000, 0.165, 0.00, 0.300,          // fa, fd, fs, fr
        0.50,                               // fenv
        0.80, 0.00, 0.00, 0.10,             // saw, pulse, sub, saw2
        0.50, 0.0,                          // pw, hpf
        0.819, 0.000, 3.127, 32,            // senv a, senv del, senv dcy, senv steps
        0.72,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.20, 0.46, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 3, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 2153, 0.13,                      // oct, freq, res
        0.000, 0.249, 0.00, 0.116,          // a, d, s, r
        0.000, 0.200, 0.00, 0.300,          // fa, fd, fs, fr
        0.37,                               // fenv
        0.38, 0.68, 0.00, 0.00,             // saw, pulse, sub, saw2
        0.49, 0.006,                        // pw, hpf
        0.500, 0.000, 0.800, 16,            // senv a, senv del, senv dcy, senv steps
        0.00,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.36, 0.13, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 0, 0.00, 0.08,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 1598, 0.00,                      // oct, freq, res
        0.000, 0.249, 0.72, 0.518,          // a, d, s, r
        0.000, 0.261, 0.00, 0.544,          // fa, fd, fs, fr
        0.47,                               // fenv
        0.80, 0.00, 0.00, 0.00,             // saw, pulse, sub, saw2
        0.50, 0.0,                          // pw, hpf
        0.000, 0.000, 5.988, 32,            // senv a, senv del, senv dcy, senv steps
        0.00,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.00, 0.00, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 3, 0.21, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 55, 0.23,                        // oct, freq, res
        0.000, 0.161, 0.00, 0.114,          // a, d, s, r
        0.000, 0.085, 0.00, 0.048,          // fa, fd, fs, fr
        0.46,                               // fenv
        0.80, 0.39, 0.00, 0.14,             // saw, pulse, sub, saw2
        0.56, 0.02,                         // pw, hpf
        0.500, 0.000, 2.941, 16,            // senv a, senv del, senv dcy, senv steps
        0.75,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        1,                                  // klok
        0.00, 0.00, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 0, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 4975, 0.35,                      // oct, freq, res
        0.000, 0.058, 0.63, 0.145,          // a, d, s, r
        0.000, 0.059, 0.00, 0.474,          // fa, fd, fs, fr
        0.81,                               // fenv
        0.27, 0.58, 0.00, 0.21,             // saw, pulse, sub, saw2
        0.50, 0.038,                        // pw, hpf
        0.273, 0.000, 0.110, 16,            // senv a, senv del, senv dcy, senv steps
        0.00,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        3,                                  // klok
        0.14, 0.20, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 0, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        1, 93, 0.08,                        // oct, freq, res
        0.000, 0.164, 0.67, 0.660,          // a, d, s, r
        0.000, 0.170, 0.00, 0.474,          // fa, fd, fs, fr
        0.22,                               // fenv
        0.40, 0.00, 0.71, 0.00,             // saw, pulse, sub, saw2
        0.52, 0.0,                          // pw, hpf
        0.450, 0.000, 0.063, 5,             // senv a, senv del, senv dcy, senv steps
        0.64,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.12, 0.22, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 0, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 125, 0.00,                       // oct, freq, res
        0.000, 0.306, 0.77, 0.053,          // a, d, s, r
        0.000, 0.197, 0.00, 0.340,          // fa, fd, fs, fr
        0.12,                               // fenv
        0.96, 0.00, 0.00, 0.00,             // saw, pulse, sub, saw2
        0.50, 0.0,                          // pw, hpf
        3.024, 1.646, 0.799, 32,            // senv a, senv del, senv dcy, senv steps
        0.14,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.35, 0.77, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 0, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    {
        0, 46, 0.16,                        // oct, freq, res
        0.000, 0.231, 0.72, 0.658,          // a, d, s, r
        0.000, 0.063, 0.00, 0.107,          // fa, fd, fs, fr
        0.15,                               // fenv
        0.00, 0.71, 0.29, 0.13,             // saw, pulse, sub, saw2
        0.25, 0.0,                          // pw, hpf
        2.519, 0.000, 0.351, 16,            // senv a, senv del, senv dcy, senv steps
        0.46,                               // senv-freq
        0.90, 0.00, 0.00,                   // out12, out3, out4
        0,                                  // klok
        0.47, 0.28, 8, 8,                   // drift-freq, drift-res, drift rate, drift loop
        0, 9, 0.00, 0.00,                   // wrap, squeeze, senv-wrap, drift-wrap
        1,                                  // midi channel [unused]
    },
    };

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
    params[PARAM_WRAP].Init("Wrap", 0, 0, 25, Parameter::EXPONENTIAL, 4);
    params[PARAM_SQUEEZE].Init("Sqz", 0, 0, 25, Parameter::EXPONENTIAL, 4);
    params[PARAM_SENV2WRAP].Init("SE>Wr", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_DR2WRAP].Init("Dr>Wr", 0, 0, 1, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_12].Init("1-2", 0.8, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_3].Init("3", 0, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_OUT_4].Init("4", 0, 0, 2, Parameter::EXPONENTIAL, 100);
    params[PARAM_MIDI_CHANNEL].Init("Midi", 7, 1, 16, Parameter::LINEAR, 1);

    // assign nullptr to leave a slot blank
    u8 p = 0;
    pages[p++].Init(Name(), "Osc", &params[PARAM_SAW], &params[PARAM_PULSE], &params[PARAM_SUB], &params[PARAM_SAW2]);
    pages[p++].Init(Name(), "Osc", &params[PARAM_OCTAVE], &params[PARAM_PULSEWIDTH], nullptr, nullptr);
    pages[p++].Init(Name(), "Osc", &params[PARAM_WRAP], &params[PARAM_SQUEEZE], &params[PARAM_SENV2WRAP], &params[PARAM_DR2WRAP]);
    pages[p++].Init(Name(), "Filt", &params[PARAM_FREQ], &params[PARAM_RES], &params[PARAM_HPF], &params[PARAM_KLOK]);
    pages[p++].Init(Name(), "Mod>Filt", &params[PARAM_F_FENV], &params[PARAM_F_SENV], &params[PARAM_F_DRIFT], &params[PARAM_FRES_DRIFT]);
    pages[p++].Init(Name(), "FEnv", &params[PARAM_FA], &params[PARAM_FD], &params[PARAM_FS], &params[PARAM_FR]);
    pages[p++].Init(Name(), "AEnv", &params[PARAM_A], &params[PARAM_D], &params[PARAM_S], &params[PARAM_R]);
    pages[p++].Init(Name(), "SyncEnv", &params[PARAM_SENV_DELAY], &params[PARAM_SENV_A], &params[PARAM_SENV_DECAY], &params[PARAM_SENV_STEPS]);
    pages[p++].Init(Name(), "Drift", &params[PARAM_DRIFT_RATE], &params[PARAM_DRIFT_LOOP], nullptr, nullptr);
    pages[p++].Init(Name(), "Out", &params[PARAM_OUT_12], &params[PARAM_OUT_3], &params[PARAM_OUT_4], &params[PARAM_MIDI_CHANNEL]);

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

    if (lastMidiChannel != (int)params[PARAM_MIDI_CHANNEL].Value()) {
        AllVoicesOff();
        lastMidiChannel = (int)params[PARAM_MIDI_CHANNEL].Value();
    }

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

    // determine if we should skip processing because of klok setting
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

    // process the envelopes before we exit for klok, so their timing isn't affected
    float syncEnvSignal = syncEnv.Process();
    float filtEnvSignal = filtEnv.Process();

    // if it's not klok time, exit
    if (k != 0) {
        // process the voice envelopes here for timing
        for (u8 voice = 0; voice < voiceLimit; voice++) {
            voices[voice].ampEnv.Process();
        }
        return 0;
    }

    float left = 0;
    float right = 0;


    float signal = 0;
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        // oscillators
        voices[voice].multiOsc.Process();
        float oscSignal = params[PARAM_SAW].Value() * voices[voice].multiOsc.Saw();
        oscSignal += params[PARAM_PULSE].Value() * voices[voice].multiOsc.Pulse();
        oscSignal += params[PARAM_SUB].Value() * voices[voice].multiOsc.Sub();
        oscSignal += params[PARAM_SAW2].Value() * voices[voice].multiOsc.Saw2();
        // signal += voices[voice].velocity * oscSignal * voices[voice].ampEnv.Process();
        float wrapValue = params[PARAM_WRAP].Value();
        wrapValue += params[PARAM_DR2WRAP].Value() * 25 * drift.Signal(2);
        wrapValue += params[PARAM_SENV2WRAP].Value() * 25 * syncEnvSignal;
        oscSignal = Folding::Wrap(oscSignal, wrapValue);
        oscSignal = Folding::Squeeze(oscSignal, params[PARAM_SQUEEZE].Value());
        signal += oscSignal * voices[voice].ampEnv.Process();
        voices[voice].isPlaying = voices[voice].ampEnv.IsRunning();
    }

    // filter
    float freq = params[PARAM_FREQ].Value();
    freq += (MAX_FILTER_FREQUENCY - MIN_FILTER_FREQUENCY) * params[PARAM_F_FENV].Value() * params[PARAM_F_FENV].Value() * filtEnvSignal;
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
            if (gates >= 1) {      // by using ">=" instead of "==" env will always retrigger
                filtEnv.GateOn();  // but with "==" env would sometimes get stuck off
                drift.Trigger();
            }
        }
    }
}

void Korra::VoiceOff(u8 voice) {
    if (voice < voiceLimit) {
        voices[voice].gateIsOn = false;
        voices[voice].ampEnv.GateOff();
    }
    // TODO: do we need to check if the voice was on and if so subtract a gate?
}

void Korra::AllVoicesOff() {
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        VoiceOff(voice);
    }
    gates = 0;
    filtEnv.GateOff();
}

void Korra::AllNotesOff() {
    AllVoicesOff();
}

void Korra::NoteOff(u8 note) {
    for (u8 voice = 0; voice < voiceLimit; voice++) {
        if (note == voices[voice].note) {
            VoiceOff(voice);
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

void Korra::Panic() {
    AllVoicesOff();
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

void Korra::MidiController(u8 cc, u8 value) { 
    if (cc >= FIRST_MIDI_CC && cc < FIRST_MIDI_CC + MIDI_CC_COUNT) {
        float fvalue = (float)value / 127.0;
        u8 param = ctrlParams[cc - FIRST_MIDI_CC];
        if (param < PARAM_COUNT) {
            params[param].Update(fvalue, true);
        }
    }
}

void Korra::LoadPreset(u8 preset) { 
    if (preset < ISYNTH_PRESET_COUNT) {
        for (u8 param = 0; param < PARAM_COUNT; param++) {
            if (param != PARAM_MIDI_CHANNEL) {
                params[param].SetScaledValue(presets[preset][param]);
            }            
        }
    }
}
