#include "Runner.h"
#include "sounds/Bd8.h"
#include "sounds/Clap.h"
#include "sounds/ClickSource.h"
#include "sounds/DigiClap.h"
#include "sounds/Sd8.h"
#include "sounds/SdNoise.h"
#include "sounds/Tom.h"

using namespace daisy;
using namespace daisysp;


#define KIT_DRUM_COUNT 8

SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

u8 drumCount = KIT_DRUM_COUNT;
IDrum *drums[KIT_DRUM_COUNT];
u8 sourceCount = 1;
IDrum *sources[1];
IDrum *midiMap[MIDIMAP_SIZE];
DrumWrapper drumWrappers[KIT_DRUM_COUNT];

// sounds
Bd8 bd;
SdNoise rs;
Sd8 sd;
Clap cp;
DigiClap sd2;
Tom lt, mt, ht;

// Shared sound sources
ClickSource clickSource;


void InitKit(float samplerate) {

    // Init any sound sources
    clickSource.Init("", samplerate, 1500, 191, 116);
    sources[0] = &clickSource;

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 2, 0.001, 0.15, 125);
    rs.Init("RS", samplerate, 0.001, 0.5, 4);
    sd.Init("SD", samplerate);
    cp.Init("CP", samplerate, 0.012, 0.8);
    sd2.Init("S2", samplerate, 0.012, 0.8, 3000, 0);

    lt.Init("LT", samplerate, 80, &clickSource);
    mt.Init("MT", samplerate, 91, &clickSource);
    ht.Init("HT", samplerate, 106, &clickSource);

    // Assign sounds to kit
    drums[0] = &bd;
    drums[1] = &rs;
    drums[2] = &sd;
    drums[3] = &cp;
    drums[4] = &sd2;
    drums[5] = &lt;
    drums[6] = &mt;
    drums[7] = &ht;

    for (u8 i = 0; i < drumCount; i++) {
        if (drums[i] != nullptr) {
            drumWrappers[i].Init(drums[i]);
            drums[i] = &drumWrappers[i];
        }
    }

    // set notes we're not using to nullptr
    for (u8 i = 0; i < MIDIMAP_SIZE; i++) {
        midiMap[i] = nullptr;
    }
    midiMap[0] = drums[0];
    midiMap[1] = drums[1];
    midiMap[2] = drums[2];
    midiMap[3] = drums[3];
    midiMap[4] = drums[4];
    midiMap[5] = drums[5];
    midiMap[7] = drums[6];
    midiMap[9] = drums[7];

}


int main(void) {

    Runner runner(daisy::SaiHandle::Config::SampleRate::SAI_32KHZ);

    float samplerate = runner.getSampleRate();

    InitKit(samplerate);

    // kit is defined in kits/ header file
    Runner::Kit kit;
    kit.drumCount = drumCount;
    kit.drums = (IDrum**)drums;
    kit.sourceCount = sourceCount;
    kit.sources = (IDrum**)sources;
    kit.midiMap = (IDrum**)midiMap;

    runner.Run(&kit);

}
