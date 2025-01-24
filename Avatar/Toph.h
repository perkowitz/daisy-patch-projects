#ifndef TOPH_H
#define TOPH_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Filters/ladder.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "lib/audio/MultiOsc.h"
#include "lib/audio/SyncEnv.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

class Toph: public ISynth {

    public:
        // pages
        static const u8 PAGE_COUNT = 8;
        u8 PageCount() { return PAGE_COUNT; }

        // params
        static const u8 PARAM_COUNT = 32;  // total count of all params following
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
        static const u8 PARAM_FENV = 11;
        static const u8 PARAM_SAW = 12;
        static const u8 PARAM_PULSE = 13;
        static const u8 PARAM_SUB = 14;
        static const u8 PARAM_SAW2 = 15;
        static const u8 PARAM_PULSEWIDTH = 16;
        static const u8 PARAM_OUT_12 = 17;
        static const u8 PARAM_OUT_3 = 18;
        static const u8 PARAM_OUT_4 = 19;
        static const u8 PARAM_SENV_A = 20;
        static const u8 PARAM_SENV_H = 21;
        static const u8 PARAM_SENV_D = 22;
        static const u8 PARAM_SENV_STEPS = 23;
        static const u8 PARAM_PITCH_SENV = 24;
        static const u8 PARAM_SENV2_A = 25;
        static const u8 PARAM_SENV2_H = 26;
        static const u8 PARAM_SENV2_D = 27;
        static const u8 PARAM_SENV2_STEPS = 28;
        static const u8 PARAM_FILT_SENV2 = 29;
        static const u8 PARAM_LFO_RATE = 30;
        static const u8 PARAM_PITCH_LFO = 31;

        // constants
        static const u16 MAX_FREQ = 24000;
        static const u16 MIN_FILTER_FREQUENCY = 0;
        static const u16 MAX_FILTER_FREQUENCY = 10000;

        void Init(float sampleRate);
        bool IsActive() { return active; }
        std::string Name() { return "Toph"; }
        std::string ShortName() { return "To"; }

        float Process();
        float GetOutput(u8 channel);
        void NoteOn(u8 note, float velocity);
        void NoteOff(u8 note);
        void Clock(u8 measure, u8 step, u8 tick);

        ParamPage *GetParamPage(u8 page);
        Param *GetParam(u8 index);
        void ResetParams(u8 page);
        void ProcessChanges() { }

        void SetMidiChannel(u8 channel) { midiChannel = channel; }
        virtual u8 GetMidiChanel() { return midiChannel; }

    private:
        bool active = false;
        float velocity = 0;
        u8 note;
        float leftSignal = 0;
        float rightSignal = 0;
        u8 activeGates = 0;
        bool retrigger = true;
        bool fourPole = true;

        Param params[PARAM_COUNT];
        ParamSet paramSets[PAGE_COUNT];
        ParamPage pages[PAGE_COUNT];
        u8 midiChannel = 1;

        MultiOsc multiOsc;
        // Svf svf;
        // Svf svf2;
        LadderFilter vcf;
        AdsrEnv ampEnv;
        AdsrEnv filtEnv;
        SyncEnv syncEnv1;
        SyncEnv syncEnv2;
        Oscillator lfo;

};



#endif
