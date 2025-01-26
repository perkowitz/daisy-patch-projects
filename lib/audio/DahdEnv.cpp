#include "DahdEnv.h"

using namespace daisy;
using namespace daisysp;

void DahdEnv::Init(float sampleRate) {
    this->sampleRate = sampleRate;
}

float DahdEnv::Process() {

    while (stage < STAGE_COUNT && counter >= stageTimes[stage]) {
        stage++;
        counter = 0;
    }

    switch (stage) {
        case STAGE_DELAY:
            signal = 0;
            break;
        case STAGE_ATTACK:
            signal = (float)counter / (float)stageTimes[STAGE_ATTACK];
            break;
        case STAGE_HOLD:
            signal = 1;
            break;
        case STAGE_DECAY:
            signal = 1 - (float)counter / (float)stageTimes[STAGE_DECAY];
            break;
        default:
            signal = 0;
    }

    counter++;

    if (curve > 0) {
        for (u8 i = 0; i < curve; i++) {
            signal *= signal;
        }
    }

    return signal;
}

void DahdEnv::Trigger() {
    stage = STAGE_DELAY;
    counter = 0;
}

void DahdEnv::SetStageTime(u8 stage, float seconds) {
    if (stage < STAGE_COUNT) {
        stageTimes[stage] = (float)sampleRate * seconds;
    }
}