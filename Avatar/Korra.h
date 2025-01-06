#ifndef KORRA_H
#define KORRA_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Filters/onepole.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "lib/audio/Drift.h"
#include "lib/audio/Folding.h"
#include "lib/audio/MultiOsc.h"
#include "lib/audio/SyncEnv.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

#define KORRA_MAX_KLOK 3
#define KORRA_KLOK_COUNT_LIMIT 8
#define KORRA_MAX_DRIFT_RATE 32

class Korra: public ISynth {

    struct Voice {
        MultiOsc multiOsc;
        AdsrEnv ampEnv;
        u8 note;
        u8 adjustedNote;
        u8 velocity;
        bool gateIsOn = false;  // whether this voice is gated on
        bool isPlaying = false; // whether this voice is still sounding (incl env release)
    };

    public:
        // polyphony
        static const u8 VOICE_COUNT = 8;

        // pages
        static const u8 PAGE_COUNT = 10;
        u8 PageCount() { return PAGE_COUNT; }

        // params
        static const u8 PARAM_COUNT = 35;  // total count of all params following
        static const u8 PARAM_OCTAVE = 0;
        static const u8 PARAM_FREQ = 1;
        static const u8 PARAM_RES = 2;
        static const u8 PARAM_A = 3;
        static const u8 PARAM_D = 4;
        static const u8 PARAM_S = 5;
        static const u8 PARAM_R = 6;
        static const u8 PARAM_FA = 7;
        static const u8 PARAM_FD = 8;
        static const u8 PARAM_FS = 9;
        static const u8 PARAM_FR = 10;
        static const u8 PARAM_F_FENV = 11;
        static const u8 PARAM_SAW = 12;
        static const u8 PARAM_PULSE = 13;
        static const u8 PARAM_SUB = 14;
        static const u8 PARAM_SAW2 = 15;
        static const u8 PARAM_PULSEWIDTH = 16;
        static const u8 PARAM_HPF = 17;
        static const u8 PARAM_TA = 18;
        static const u8 PARAM_TH = 19;
        static const u8 PARAM_TD = 20;
        static const u8 PARAM_SENV_STEPS = 21;
        static const u8 PARAM_F_SENV = 22;
        static const u8 PARAM_OUT_12 = 23;
        static const u8 PARAM_OUT_3 = 24;
        static const u8 PARAM_OUT_4 = 25;
        static const u8 PARAM_KLOK = 26;
        static const u8 PARAM_F_DRIFT = 27;
        static const u8 PARAM_FRES_DRIFT = 28;
        static const u8 PARAM_DRIFT_RATE = 29;
        static const u8 PARAM_DRIFT_LOOP = 30;
        static const u8 PARAM_FOLD = 31;
        static const u8 PARAM_WRAP = 32;
        static const u8 PARAM_SQUEEZE = 33;
        static const u8 PARAM_DR2FOLD = 34;

        // constants
        static const u16 MAX_FREQUENCY = 24000;
        static const u16 MIN_FILTER_FREQUENCY = 0;
        static const u16 MAX_FILTER_FREQUENCY = 10000;

        void Init(float sampleRate);
        void Init(float sampleRate, u8 voiceLimit);
        bool IsActive() { return active; }
        std::string Name() { return "Korra"; }
        std::string ShortName() { return "Kor"; }

        float Process();
        float GetOutput(u8 channel);
        void NoteOn(u8 note, float velocity);
        void NoteOff(u8 note);
        void Clock(u8 measure, u8 step, u8 tick);

        ParamPage *GetParamPage(u8 page);
        Param *GetParam(u8 index);
        void ResetParams(u8 page);
        void ProcessChanges();

        void SetMidiChannel(u8 channel) { midiChannel = channel; }
        virtual u8 GetMidiChanel() { return midiChannel; }

        float GetDrift(u8 step) { return drift.Value(step); }

    private:
        bool active = false;
        float leftSignal = 0;
        float rightSignal = 0;
        u8 gates = 0;

        Param params[PARAM_COUNT];
        ParamSet paramSets[PAGE_COUNT];
        ParamPage pages[PAGE_COUNT];

        Voice voices[VOICE_COUNT];
        u8 nextVoice = 0;
        u8 midiChannel = 1;
        u8 voiceLimit = VOICE_COUNT;
        u8 klokCount = 0;
        u8 currentKlok = 0;

        OnePole hpf;
        SyncEnv syncEnv;
        Svf svf;
        AdsrEnv filtEnv;
        Drift drift;

};



#endif
