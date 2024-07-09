#include "DrumWrapper.h"

using namespace daisy;
using namespace daisysp;

float DrumWrapper::Process() {
    if (!IsActive() && cycleCount > CYCLECOUNT_THRESHOLD) return 0.0f;

    cycleCount++;
    return drum->Process();
}

void DrumWrapper::Trigger(float velocity) {
    cycleCount = 0;
    drum->Trigger(velocity);
}

