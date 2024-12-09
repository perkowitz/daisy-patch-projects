#include "Runner.h"
#include "Simple.h"

using namespace daisy;
using namespace daisysp;


#define KIT_DRUM_COUNT 8

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    Simple simple;

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);

    float sampleRate = runner.getSampleRate();
    simple.Init(sampleRate);

    runner.Run(&simple);
}
