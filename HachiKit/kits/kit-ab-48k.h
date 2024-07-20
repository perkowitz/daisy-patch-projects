// all kit .h files should include this so only one can get loaded
#ifndef DRUMKIT_H
#define DRUMKIT_H

#include "../sounds/Bd8.h"
#include "../sounds/Clap.h"
#include "../sounds/ClickSource.h"
#include "../sounds/DigiClap.h"
#include "../sounds/Sd8.h"
#include "../sounds/SdNoise.h"
#include "../sounds/Tom.h"


SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;

IDrum *drums[8];
IDrum *sources[1];
u8 drumCount = 8;
u8 sourceCount = 1;
DrumWrapper drumWrappers[8];

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
    bd.Init("BD", samplerate, 64, 0.001, 4, 0.001, 0.15, 125);
    rs.Init("RS", samplerate);
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
        drumWrappers[i].Init(drums[i]);
        drums[i] = &drumWrappers[i];
    }
    
}

#endif
