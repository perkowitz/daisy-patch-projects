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

// #define SELECTED_KIT 0

// #if SELECTED_KIT == 0
// #include "kits/kit-abcd.h"
// #endif

#include "kits/kit-abcd.h"
// #include "kits/testing.h"

#endif
