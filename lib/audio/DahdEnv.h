#ifndef DAHDENV_H
#define DAHDENV_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class DahdEnv {

    public:
        static const u8 STAGE_COUNT = 4;
        static const u8 STAGE_DELAY = 0;
        static const u8 STAGE_ATTACK = 1;
        static const u8 STAGE_HOLD = 2;
        static const u8 STAGE_DECAY = 3;

        void Init(float sampleRate);
        float Process();
        void Trigger();
        void SetStageTime(u8 stage, float seconds);
        void SetCurve(u8 curve) { this->curve = std::min(10, (int)curve); }
        bool IsRunning() { return stage < STAGE_COUNT; }

    private:
        float sampleRate = 48000;
        long stageTimes[STAGE_COUNT] = { 0, 0, 100, 1000 };
        u8 curve = 0;
        long counter = 0;
        u8 stage = STAGE_COUNT;
        float signal;
};



#endif
