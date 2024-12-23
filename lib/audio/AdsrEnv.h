#ifndef ADSRENV_H
#define ADSRENV_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class AdsrEnv {

    public:
        static const u8 STAGE_COUNT = 4;
        static const u8 STAGE_ATTACK = 0;
        static const u8 STAGE_DECAY = 1;
        static const u8 STAGE_SUSTAIN = 2;
        static const u8 STAGE_RELEASE = 3;

        void Init(float sampleRate);
        float Process();
        void GateOn();
        void GateOff();
        void GateOff(bool force);
        void SetStageTime(u8 stage, float seconds);
        void SetCurve(u8 curve) { this->curve = std::min(10, (int)curve); }
        void SetSustainLevel(float level) { sustainLevel = level; }
        bool IsRunning() { return stage < STAGE_COUNT; }

    private:
        float sampleRate = 48000;
        long stageTimes[STAGE_COUNT] = { 1, 100, 1, 1000 };  // stagetime doesn't matter for sustain
        u8 curve = 0;
        bool retrigger = true;
        u8 activeGates = 0;
        float sustainLevel = 1;

        long counter = 0;
        u8 stage = STAGE_COUNT;
        float signal = 0;
        float gateOffSignal = 0;
};



#endif
