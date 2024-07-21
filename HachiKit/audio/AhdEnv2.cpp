#include "AhdEnv2.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

void AhdEnv2::Init(float sampleRate) {
    this->sampleRate = sampleRate;
}

float AhdEnv2::Process() {

    while (stage < STAGE_COUNT && counter >= stageTimes[stage]) {
        stage++;
        counter = 0;
    }

    switch (stage) {
        case STAGE_ATTACK:
            signal = (float)counter / (float)stageTimes[0];
            break;
        case STAGE_HOLD:
            signal = 1;
            break;
        case STAGE_DECAY:
            signal = 1 - (float)counter / (float)stageTimes[2];
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

void AhdEnv2::Trigger() {
    stage = STAGE_ATTACK;
    counter = 0;
}

void AhdEnv2::SetStageTime(u8 stage, float seconds) {
    if (stage < STAGE_COUNT) {
        stageTimes[stage] = sampleRate * seconds;
    }
}