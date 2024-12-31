#ifndef ZUKO_H
#define ZUKO_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Filters/onepole.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "lib/audio/SyncMultiOsc.h"
#include "lib/audio/WaveSyncOsc.h"
#include "lib/audio/SyncEnv.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

#define ZUKO_MAX_KLOK 3
#define ZUKO_KLOK_COUNT_LIMIT 8
#define ZUKO_BUFFER_SIZE 1024

class Zuko: public ISynth {

    struct Voice {
        SyncMultiOsc syncMultiOsc;
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
        static const u8 PAGE_COUNT = 8;
        u8 PageCount() { return PAGE_COUNT; }

        // params
        static const u8 PARAM_COUNT = 30;  // total count of all params following
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
        static const u8 PARAM_SAMPLE_START = 12;
        static const u8 PARAM_SAW = 13;
        static const u8 PARAM_OSC2_SAW = 14;
        static const u8 PARAM_OSC2_PULSE = 15;
        static const u8 PARAM_OSC2_SYNC = 16;
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
        static const u8 PARAM_OSC1_LEVEL = 27;
        static const u8 PARAM_OSC2_LEVEL = 28;
        static const u8 PARAM_OSC2_OCTAVE = 29;

        // constants
        static const u16 MAX_FREQ = 24000;

        void Init(float sampleRate);
        void Init(float sampleRate, u8 voiceLimit);
        bool IsActive() { return active; }
        std::string Name() { return "Zuko"; }
        std::string ShortName() { return "Zu"; }

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

        float waveSamples[ZUKO_BUFFER_SIZE];

        OnePole hpf;
        SyncEnv syncEnv;
        Svf svf;
        AdsrEnv filtEnv;

};



#endif
