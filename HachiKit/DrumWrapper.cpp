#include "DrumWrapper.h"

using namespace daisy;
using namespace daisysp;

float DrumWrapper::Process() {
    // If we're at a sample that we've previously filled, return the sample instead of processing.
    if (bufferEnabled && bufferIndex < bufferFilledIndex) {
        if (bufferIndex == BUFFER_SIZE-1) {
            bufferDone = true;
        }
        return buffer[bufferIndex++];
    }

    // If we've reached the end of the buffer, play no more sound.
    if (bufferEnabled && bufferDone) {
        return 0;
    }

    // If the drum is inactive (i.e. done) and we're beyond the first few samples, don't process.
    if (!IsActive() && cycleCount > CYCLECOUNT_THRESHOLD) return 0.0f;

    cycleCount++;

    // Otherwise, process.
    float sig = drum->Process();

    // If we haven't filled this part of the buffer yet, fill it.
    if (bufferEnabled && bufferIndex >= bufferFilledIndex && bufferFilledIndex < BUFFER_SIZE) {
        buffer[bufferFilledIndex] = sig;
        if (bufferFilledIndex > BUFFER_SIZE - BUFFER_FADEOUT) {
            buffer[bufferFilledIndex] *= (BUFFER_SIZE - bufferFilledIndex - 0.0f) / (BUFFER_FADEOUT + 0.0f);
        }
        bufferFilledIndex++;
    }
    
    bufferIndex++;
    return sig;
}

void DrumWrapper::Trigger(float velocity) {
    cycleCount = 0;
    bufferIndex = 0;
    bufferDone = false;
    drum->Trigger(velocity);
}

