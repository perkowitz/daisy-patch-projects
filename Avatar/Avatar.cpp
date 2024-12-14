#include "Runner.h"
#include "Toph.h"

using namespace daisy;
using namespace daisysp;


#define KIT_DRUM_COUNT 8

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    Toph toph;

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);

    float sampleRate = runner.getSampleRate();
    toph.Init(sampleRate);

    runner.Run(&toph);
}
