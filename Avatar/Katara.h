#ifndef KATARA_H
#define KATARA_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Filters/onepole.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "lib/audio/MultiOsc.h"
#include "lib/audio/SyncEnv.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

class Katara: public ISynth {

    struct Voice {
        MultiOsc multiOsc;
        Svf svf;
        AdsrEnv ampEnv;
        AdsrEnv filtEnv;
        u8 note;
        u8 adjustedNote;
        u8 velocity;
        bool gateOn;
    };

    public:
        // polyphony
        static const u8 VOICE_COUNT = 5;

        // pages
        static const u8 PAGE_COUNT = 8;
        u8 PageCount() { return PAGE_COUNT; }

        // params
        static const u8 PARAM_COUNT = 26;  // total count of all params following
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
        static const u8 PARAM_HPF = 17;
        static const u8 PARAM_TA = 18;
        static const u8 PARAM_TH = 19;
        static const u8 PARAM_TD = 20;
        static const u8 PARAM_SENV_STEPS = 21;
        static const u8 PARAM_F_SENV = 22;
        static const u8 PARAM_OUT_12 = 23;
        static const u8 PARAM_OUT_3 = 24;
        static const u8 PARAM_OUT_4 = 25;

        // constants
        static const u16 MAX_FREQ = 24000;
        static const u8 MIX_SCALE = 1;

        void Init(float sampleRate);
        void Init(float sampleRate, u8 voiceLimit, bool filterEnabled);
        bool IsActive() { return active; }
        std::string Name() { return "Katara"; }
        std::string ShortName() { return "Kat"; }

        float Process();
        float GetOutput(u8 channel);
        void NoteOn(u8 note, float velocity);
        void NoteOff(u8 note);
        void Clock(u8 measure, u8 step, u8 tick);
        void MidiController(u8 cc, u8 value) { }
        void Panic() { }

        ParamPage *GetParamPage(u8 page);
        Param *GetParam(u8 index);
        void ResetParams(u8 page);
        void ProcessChanges();

        void SetMidiChannel(u8 channel) { midiChannel = channel; }
        virtual u8 GetMidiChannel() { return midiChannel; }

        void LoadPreset(u8 preset) { }

    private:
        bool active = false;
        float leftSignal = 0;
        float rightSignal = 0;

        Param params[PARAM_COUNT];
        ParamSet paramSets[PAGE_COUNT];
        ParamPage pages[PAGE_COUNT];

        Voice voices[VOICE_COUNT];
        u8 nextVoice = 0;
        u8 midiChannel = 1;
        u8 voiceLimit = VOICE_COUNT;    // max number of voices to use of those available (reduce processing)
        bool filterEnabled = true;      // disable per-voice LPFs to reduce processing

        OnePole hpf;
        SyncEnv syncEnv;

};



#endif
