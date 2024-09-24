#include "Runner.h"
#include "sounds/Bd8.h"

using namespace daisy;
using namespace daisysp;


SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_32KHZ;

IDrum *drums[16];
IDrum *sources[1];
u8 drumCount = 16;
u8 sourceCount = 0;

// sounds
Bd8 bd;



void InitKit(float samplerate) {

    // Init any sound sources

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 4, 1.001, 0.15, 125);

    // Assign sounds to kit
    for (u8 i = 1; i < drumCount; i++) {
        drums[i] = nullptr;
    }
    DrumWrapper wrapper;
    wrapper.Init(&bd);
    drums[0] = &wrapper;
}


int main(void) {

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_32KHZ);

    float samplerate = runner.getSampleRate();

    InitKit(samplerate);

    // kit is defined in kits/ header file
    Runner::Kit kit;
    kit.drumCount = 16;
    kit.drums = (IDrum**)drums;
    kit.sourceCount = 0;
    kit.sources = (IDrum**)sources;

    runner.Run(&kit);

}
