#ifndef SYNCENV_H
#define SYNCENV_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "AhdEnv.h"

using namespace daisy;
using namespace daisysp;

class SyncEnv {

    public:
        void Init(float sampleRate) { env.Init(sampleRate); }
        float Process() { return env.Process(); }
        void Trigger() { env.Trigger(); }
        AhdEnv *GetEnv() { return &env; }
        void SetStageTime(u8 stage, float seconds) { env.SetStageTime(stage, seconds); }
        void SetCurve(u8 curve) { env.SetCurve(curve); }
        void SetSyncSteps(u8 syncSteps) { this->syncSteps = syncSteps; }

        // makes the assumption that steps = 16ths
        void Clock(u8 measure, u8 step, u8 tick) {
            if (syncSteps == 0) return;
            if (tick != 0) return;

            // stepCount++;
            // if (stepCount >= syncSteps) {
            //     Trigger();
            //     stepCount = 0;
            // }

            u16 totalSteps = measure * 16 + step;
            if (totalSteps % syncSteps == 0) {
                Trigger();
            }

        }

    private:
        AhdEnv env;
        u8 syncSteps = 16;
        u8 stepCount = 0;
};



#endif
