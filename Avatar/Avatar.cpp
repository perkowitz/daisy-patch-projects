#include "Runner.h"
#include "Toph.h"
#include "Katara.h"

using namespace daisy;
using namespace daisysp;

#define SYNTH_COUNT 2

Katara katara;
Toph toph;


SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
    float sampleRate = runner.getSampleRate();

    katara.Init(sampleRate);
    katara.SetMidiChannel(4); // zero-indexed
    toph.Init(sampleRate);
    toph.SetMidiChannel(5); // zero-indexed

    // runner.Run(&katara, &toph);
    runner.Run(&toph, nullptr);
}
