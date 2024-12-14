#include "AdsrEnv.h"

using namespace daisy;
using namespace daisysp;

void AdsrEnv::Init(float sampleRate) {
    this->sampleRate = sampleRate;
}

float AdsrEnv::Process() {

    while (stage < STAGE_COUNT && stage != STAGE_SUSTAIN && counter >= stageTimes[stage]) {
        stage++;
        counter = 0;
    }

    switch (stage) {
        case STAGE_ATTACK:
            signal = (float)counter / (float)stageTimes[STAGE_ATTACK];
            break;
        case STAGE_DECAY:
            signal = 1 - (float)counter / (float)stageTimes[STAGE_DECAY]; 
            signal = signal * (1 - sustainLevel) + sustainLevel;
            break;
        case STAGE_SUSTAIN:
            signal = sustainLevel;
            break;
        case STAGE_RELEASE:
            signal = 1 - (float)counter / (float)stageTimes[STAGE_RELEASE]; 
            signal = signal * gateOffSignal;
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

void AdsrEnv::GateOn() {
    if (retrigger || activeGates == 0) {
        stage = STAGE_ATTACK;
        counter = 0;
    }
    activeGates++;
}

void AdsrEnv::GateOff() {
    GateOff(false);
}


void AdsrEnv::GateOff(bool force) {
    if (force) {
        activeGates = 0;
    } else if (activeGates > 0) {
        activeGates--;
    }
    if (activeGates == 0) {
        gateOffSignal = signal;
        stage = STAGE_RELEASE;
        counter = 0;
    }
}

void AdsrEnv::SetStageTime(u8 stage, float seconds) {
    if (stage < STAGE_COUNT) {
        stageTimes[stage] = (float)sampleRate * seconds;
    }
}