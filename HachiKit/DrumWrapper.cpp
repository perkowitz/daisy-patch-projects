#include "DrumWrapper.h"

using namespace daisy;
using namespace daisysp;

float DrumWrapper::Process() {
    if (drum == nullptr) { return 0; }

    // If the drum is inactive (i.e. done) and we're beyond the first few samples, don't process.
    if (!IsActive() && cycleCount > CYCLECOUNT_THRESHOLD) return 0.0f;

    cycleCount++;
    return drum->Process();
}

void DrumWrapper::Trigger(float velocity) {
    if (drum == nullptr) { return; }

    cycleCount = 0;
    drum->Trigger(velocity);
}

float DrumWrapper::UpdateParam(uint8_t param, float value) { 
    if (drum == nullptr) { return 0; }
    return drum->UpdateParam(param, value);
}

void DrumWrapper::SetParam(uint8_t param, float value) { 
    if (drum == nullptr) { return; }
    drum->SetParam(param, value);
}
