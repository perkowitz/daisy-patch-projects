#include "Runner.h"
#include "sounds/Ch.h"
#include "sounds/Clave8.h"
#include "sounds/Cow8.h"
#include "sounds/Cy.h"
#include "sounds/FmDrum.h"
#include "sounds/HhSource68.h"
#include "sounds/Oh.h"
#include "sounds/Sd8.h"

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
Ch ch;
Oh oh;
Sd8 ma;
Cy cy;
Cow8 cb;
FmDrum fm1, fm2;
Clave8 cl;

// Shared sound sources
HhSource68 source68;


void InitKit(float samplerate) {

    // Init any sound sources
    source68.Init("", samplerate, HhSource68::MORPH_808_VALUE);
    sources[0] = &source68;

    // Init all drum sounds
    ch.Init("CH", samplerate, 0.001, 0.5, &source68, HhSource68::MORPH_808_VALUE, 3000, 16000);
    oh.Init("OH", samplerate, 0.001, 0.13, 0.001, &source68, HhSource68::MORPH_808_VALUE, 3000, 16000);
    ma.Init("MA", samplerate);
    cy.Init("CY", samplerate, 0.001, 3.5, &source68, 1700, 2400);
    cb.Init("CB", samplerate, 0.005, 0.5, &source68, 1700, 2400);
    fm1.Init("LC", samplerate, 98, 3.3, 2.2, 0.001, 0.101, -50);
    fm2.Init("HC", samplerate, 131, 3.3, 2.2, 0.001, 0.101, -50);
    cl.Init("CL", samplerate, 2000, 0.375);

    // Assign sounds to kit
    drums[0] = &ch;
    drums[1] = &ma;
    drums[2] = &oh;
    drums[3] = &fm1;
    drums[4] = &fm2;
    drums[5] = &cy;
    drums[6] = &cl;
    drums[7] = &cb;

    // set notes we're not using to nullptr
    for (u8 i = 0; i < MIDIMAP_SIZE; i++) {
        midiMap[i] = nullptr;
    }
    midiMap[6] = drums[0];
    midiMap[8] = drums[1];
    midiMap[10] = drums[2];
    midiMap[11] = drums[3];
    midiMap[12] = drums[4];
    midiMap[13] = drums[5];
    midiMap[14] = drums[6];
    midiMap[15] = drums[7];

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
