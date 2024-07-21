/**
 * Kits
 * 
 * Sets up all the drum sounds and the kit.
 * Uses compiler directives to avoid extra overhead.
 * Must include these declarations:
 *   IDrum *drums[N];
 *   IDrum *sources[M];
 *   u8 drumCount = N;
 *   u8 sourceCount = M;
 *   void InitKit(float samplerate) {}
 *   SaiHandle::Config::SampleRate audioSampleRate = SaiHandle::Config::<rate>;
*/
#ifndef KITS_H
#define KITS_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "IDrum.h"
#include "Utility.h"


// #define SELECTED_KIT 0

// #if SELECTED_KIT == 0
// #include "kits/kit-abcd.h"
// #endif

// #include "kits/testing.h"
#include "kits/kit-abcd-32k.h"
// #include "kits/kit-ab-48k.h"

#endif
