#ifndef SIMPLE_H
#define SIMPLE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "lib/audio/MultiOsc.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

class Simple: public ISynth {

    public:
        // pages
        static const u8 PAGE_COUNT = 5;
        u8 PageCount() { return PAGE_COUNT; }

        // params
        static const u8 PARAM_COUNT = 17;  // total count of all params following
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

        // constants
        static const u16 MAX_FREQ = 24000;

        void Init(float sampleRate);
        bool IsActive() { return active; }
        std::string Name() { return "Simple"; }
        std::string ShortName() { return "Sim"; }

        float Process();
        float GetOutput(u8 channel);
        void NoteOn(u8 note, float velocity);
        void NoteOff(u8 note);

        ParamPage *GetParamPage(u8 page);
        void ResetParams(u8 page);

    private:
        bool active = false;
        float velocity = 0;
        float leftSignal = 0;
        float rightSignal = 0;
        u8 activeGates = 0;
        bool retrigger = true;
        bool fourPole = true;

        Param params[PARAM_COUNT];
        ParamSet paramSets[PAGE_COUNT];
        ParamPage pages[PAGE_COUNT];

        MultiOsc multiOsc;
        Svf svf;
        Svf svf2;
        AdsrEnv ampEnv;
        AdsrEnv filtEnv;

};



#endif
