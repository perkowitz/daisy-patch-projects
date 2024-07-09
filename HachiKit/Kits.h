/**
 * Kits
 * 
 * Sets up all the drum sounds and the kit.
 * Uses compiler directives to avoid extra overhead.
 * Must include these declarations:
 *   IDrum *drums[N];
 *   u8 drumCount = N;
 *   void InitKit(float samplerate) {}
 * 
*/
#ifndef KITS_H
#define KITS_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "IDrum.h"
#include "Utility.h"
#include "sounds/Bd8.h"
#include "sounds/Ch.h"
#include "sounds/Clap.h"
#include "sounds/Clave8.h"
#include "sounds/ClickSource.h"
#include "sounds/Cow8.h"
#include "sounds/Cy.h"
#include "sounds/DigiClap.h"
#include "sounds/FmDrum.h"
#include "sounds/HhSource68.h"
#include "sounds/MultiTomSource.h"
#include "sounds/MultiTom.h"
#include "sounds/Oh.h"
#include "sounds/Sd8.h"
#include "sounds/SdNoise.h"
#include "sounds/Tom.h"

#define SELECTED_KIT 0

#if SELECTED_KIT == 0

IDrum *drums[16];
u8 drumCount = 16;
DrumWrapper drumWrappers[16];

Bd8 bd;
DrumWrapper bdWrapper;
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

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 4, 0.001, 0.15, 125);
    bdWrapper.Init(&bd);
    rs.Init("RS", samplerate);
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
    ma.Init("MA", samplerate);
    cy.Init("CY", samplerate, 0.001, 3.5, &source68, 1700, 2400);
    cb.Init("CB", samplerate, 0.005, 0.5, &source68, 1700, 2400);
    fm1.Init("LC", samplerate, 98, 3.3, 2.2, 0.001, 0.101, -50);
    fm2.Init("HC", samplerate, 131, 3.3, 2.2, 0.001, 0.101, -50);
    cl.Init("CL", samplerate, 2000, 0.375);

    // Assign sounds to kit
    drums[0] = &bdWrapper;
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
}

#endif // kit==0


#endif
