#include "Runner.h"
#include "Toph.h"
#include "Katara.h"

using namespace daisy;
using namespace daisysp;


#define KIT_DRUM_COUNT 8

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    // Toph synth;
    Katara synth;
    

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);

    float sampleRate = runner.getSampleRate();
    synth.Init(sampleRate);

    runner.Run(&synth);
}
