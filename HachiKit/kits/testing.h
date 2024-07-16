// all kit .h files should include this so only one can get loaded
#ifndef DRUMKIT_H
#define DRUMKIT_H

#include "../sounds/Bd8.h"
#include "../sounds/Ch.h"
#include "../sounds/HhSource68.h"


SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_32KHZ;

IDrum *drums[2];
IDrum *sources[1];
u8 drumCount = 2;
u8 sourceCount = 1;
DrumWrapper drumWrappers[1];

// sounds
Bd8 bd;
Ch ch;

// Shared sound sources
HhSource68 source68;


void InitKit(float samplerate) {

    // Init any sound sources
    source68.Init("", samplerate, HhSource68::MORPH_808_VALUE);
    sources[0] = &source68;

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 4, 0.001, 0.15, 125);
    ch.Init("CH", samplerate, 0.001, 0.5, &source68, HhSource68::MORPH_808_VALUE, 6000, 16000);

    // Assign sounds to kit
    drums[0] = &bd;
    drums[1] = &ch;

    for (u8 i = 0; i < drumCount; i++) {
        drumWrappers[i].Init(drums[i]);
        drums[i] = &drumWrappers[i];
    }

}

#endif
