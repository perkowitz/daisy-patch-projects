#ifndef SIMPLE_H
#define SIMPLE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "lib/ParamSet.h"
#include "lib/audio/AdsrEnv.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;

class Simple: public ISynth {

    public:
        // Param pages and params
        static const u8 PAGE_COUNT = 1;
        static const u8 PARAM_COUNT = 4;
        u8 PageCount() { return PAGE_COUNT; }

        void Init(float sampleRate) {
            params[0].Init("Pitch", 440, 20, 5000, Parameter::EXPONENTIAL, 1);
            params[1].Init("Atk", 0.05, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
            params[2].Init("Dcy", 0.3, 0.0, 5.0, Parameter::EXPONENTIAL, 1000);
            params[3].Init("", 0, -1, 1, Parameter::LINEAR, 10);
            pages[0].Init(4, &params[0]);

            // oscillator settings
            osc.Init(sampleRate);
            osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
            osc.SetFreq(440);
            osc.SetAmp(1);

            // filter settings
            svf.Init(sampleRate);
            svf.SetRes(0.05);
            svf.SetDrive(.002);
            svf.SetFreq(5000);

            // ampEnv settings
            ampEnv.Init(sampleRate);
            ampEnv.SetStageTime(AdsrEnv::STAGE_ATTACK, 0);
            ampEnv.SetStageTime(AdsrEnv::STAGE_DECAY, 0.05);
            ampEnv.SetStageTime(AdsrEnv::STAGE_RELEASE, 0.2);
            ampEnv.SetSustainLevel(00.8);

        }

        bool IsActive() { return active; }
        std::string Name() { return "Simple"; }

        float Process();
        float GetOutput(u8 channel);
        void NoteOn(u8 note, float velocity);
        void NoteOff(u8 note);

        ParamSet *GetParamSet(u8 page);
        std::string GetPageName(u8 page);
        void ResetParams(u8 page);

    private:
        bool active = false;
        float velocity = 0;
        float leftSignal = 0;
        float rightSignal = 0;

        Param params[PARAM_COUNT];
        ParamSet pages[PAGE_COUNT];
        std::string pageNames[PAGE_COUNT] = { "Main" };

        Oscillator osc;
        Svf svf;
        AdsrEnv ampEnv;
        u8 activeGates = 0;
        bool retrigger = true;


};



#endif
