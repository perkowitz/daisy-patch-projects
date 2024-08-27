// all kit .h files should include this so only one can get loaded
#ifndef DRUMKIT_H
#define DRUMKIT_H

#include "../sounds/Bd8.h"
#include "../sounds/Ch.h"
#include "../sounds/Clap.h"
#include "../sounds/Clave8.h"
#include "../sounds/ClickSource.h"
#include "../sounds/Cow8.h"
#include "../sounds/Cy.h"
#include "../sounds/DigiClap.h"
#include "../sounds/FmDrum.h"
#include "../sounds/HhSource68.h"
#include "../sounds/MultiTomSource.h"
#include "../sounds/MultiTom.h"
#include "../sounds/Oh.h"
#include "../sounds/Sd8.h"
#include "../sounds/SdNoise.h"
#include "../sounds/Tom.h"


SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::SampleRate::SAI_32KHZ;

IDrum *drums[16];
IDrum *sources[2];
u8 drumCount = 16;
u8 sourceCount = 2;
DrumWrapper drumWrappers[16];

// sounds
Bd8 bd;
SdNoise rs;
Sd8 sd;
Clap cp;
DigiClap sd2;
Tom lt, mt, ht;
// MultiTom lt, mt, ht;
Ch ch;
Oh oh;
SdNoise ma;
Cy cy;
Cow8 cb;
FmDrum fm1, fm2;
Clave8 cl;

// Shared sound sources
HhSource68 source68;
ClickSource clickSource;
// MultiTomSource multiTomSource;


void InitKit(float samplerate) {

    // Init any sound sources
    source68.Init("", samplerate, HhSource68::MORPH_808_VALUE);
    clickSource.Init("", samplerate, 1500, 191, 116);
    // multiTomSource.Init("", samplerate, 500, &clickSource);
    sources[0] = &source68;
    sources[1] = &clickSource;

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 4, 1.001, 0.15, 125);
    rs.Init("RS", samplerate, 0.2, 0.5, 4);
    sd.Init("SD", samplerate);
    cp.Init("CP", samplerate, 0.012, 0.8);
    sd2.Init("S2", samplerate, 0.012, 0.8, 3000, 0);

    lt.Init("LT", samplerate, 80, &clickSource);
    mt.Init("MT", samplerate, 91, &clickSource);
    ht.Init("HT", samplerate, 106, &clickSource);
    // lt.Init("LT", samplerate, 80, 0.8, &multiTomSource, 0);
    // mt.Init("MT", samplerate, 91, 0.8, &multiTomSource, 1);
    // ht.Init("HT", samplerate, 106, 0.8, &multiTomSource, 2);


    ch.Init("CH", samplerate, 0.001, 0.5, &source68, HhSource68::MORPH_808_VALUE, 6000, 16000);
    oh.Init("OH", samplerate, 0.001, 0.13, 0.001, &source68, HhSource68::MORPH_808_VALUE, 6000, 16000);
    ma.Init("MA", samplerate, 0, 0.2, 8);
    cy.Init("CY", samplerate, 0.001, 3.5, &source68, 1700, 2400);
    cb.Init("CB", samplerate, 0.005, 0.5, &source68, 1700, 2400);
    fm1.Init("LC", samplerate, 98, 3.3, 2.2, 0.001, 0.101, -50);
    fm2.Init("HC", samplerate, 131, 3.3, 2.2, 0.001, 0.101, -50);
    cl.Init("CL", samplerate, 2000, 0.375);

    // Assign sounds to kit
    drums[0] = &bd;
    drums[1] = &rs;
    drums[2] = &sd;
    drums[3] = &cp;
    drums[4] = &sd2;
    drums[5] = &lt;
    drums[6] = &ch;
    drums[7] = &mt;
    drums[8] = &ma;
    drums[9] = &ht;
    drums[10] = &oh;
    drums[11] = &fm1;
    drums[12] = &fm2;
    drums[13] = &cy;
    drums[14] = &cl;
    drums[15] = &cb;

    for (u8 i = 0; i < drumCount; i++) {
        drumWrappers[i].Init(drums[i]);
        drums[i] = &drumWrappers[i];
    }
    
    // buffers don't work well with claps; cymbal is just too long
    // drumWrappers[3].setBufferEnabled(false);
    // drumWrappers[4].setBufferEnabled(false);
    // drumWrappers[13].setBufferEnabled(false);
}

#endif
