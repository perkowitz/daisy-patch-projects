#include "Runner.h"
#include "Asami.h"
#include "Katara.h"
#include "Korra.h"
#include "Toph.h"

using namespace daisy;
using namespace daisysp;


// Asami asami;
Katara katara;
Korra korra;
Toph toph;

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

int main(void) {

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
    float sampleRate = runner.getSampleRate();
    // const char *sdPath = runner.getSdPath();

    katara.Init(sampleRate, 4, true);
    katara.SetMidiChannel(4); // zero-indexed
    katara.GetParam(Katara::PARAM_OUT_12)->SetScaledValue(0.8);
    katara.GetParam(Katara::PARAM_OUT_3)->SetScaledValue(0);
    katara.GetParam(Katara::PARAM_OUT_4)->SetScaledValue(0);

    korra.Init(sampleRate, 8);
    korra.SetMidiChannel(4); // zero-indexed
    korra.GetParam(Katara::PARAM_OUT_12)->SetScaledValue(0.8);
    korra.GetParam(Katara::PARAM_OUT_3)->SetScaledValue(0);
    korra.GetParam(Katara::PARAM_OUT_4)->SetScaledValue(0);

    // asami.Init(sampleRate, sdPath, 1);
    // asami.SetMidiChannel(4); // zero-indexed
    // asami.GetParam(Katara::PARAM_OUT_12)->SetScaledValue(0.8);
    // asami.GetParam(Katara::PARAM_OUT_3)->SetScaledValue(0);
    // asami.GetParam(Katara::PARAM_OUT_4)->SetScaledValue(0);

    // toph.Init(sampleRate);
    // toph.SetMidiChannel(5); // zero-indexed
    // toph.GetParam(Toph::PARAM_OUT_12)->SetScaledValue(0);
    // toph.GetParam(Toph::PARAM_OUT_3)->SetScaledValue(0.8);
    // toph.GetParam(Toph::PARAM_OUT_4)->SetScaledValue(0);

    // runner.Run(&katara, nullptr);
    runner.Run(&korra, nullptr);
    // runner.Run(&asami, nullptr);
    // runner.Run(&toph, nullptr);
    // runner.Run(&katara, &toph);
}
