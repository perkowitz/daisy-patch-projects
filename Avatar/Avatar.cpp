#include "Runner.h"
#include "Toph.h"
#include "Katara.h"

using namespace daisy;
using namespace daisysp;


Katara katara;
Toph toph;

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
    float sampleRate = runner.getSampleRate();

    katara.Init(sampleRate, 4);
    katara.SetMidiChannel(4); // zero-indexed
    katara.GetParam(Katara::PARAM_OUT_12)->SetScaledValue(0.8);
    katara.GetParam(Katara::PARAM_OUT_3)->SetScaledValue(0);
    katara.GetParam(Katara::PARAM_OUT_4)->SetScaledValue(0);

    toph.Init(sampleRate);
    toph.SetMidiChannel(5); // zero-indexed
    toph.GetParam(Toph::PARAM_OUT_12)->SetScaledValue(0);
    toph.GetParam(Toph::PARAM_OUT_3)->SetScaledValue(0.8);
    toph.GetParam(Toph::PARAM_OUT_4)->SetScaledValue(0);

    runner.Run(&katara, nullptr);
    // runner.Run(&toph, nullptr);
    // runner.Run(&katara, &toph);
}
