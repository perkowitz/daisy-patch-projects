/**
 * Storage
 * 
 * Patch storage:
 * - 8 float params per sound
 *   = 4 bytes * 8 params * 8 sounds * 8 patches = 2048 bytes
 * - actual float params per sound
 *   = 4 bytes * ~4 params * 8 sounds * 8 patches = 1024 bytes
 * 
*/
#ifndef STORAGE_H
#define STORAGE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Utility.h"
#include "Runner.h"

using namespace daisy;
using namespace daisysp;

#define CURRENT_VERSION 0
#define PATCH_SIZE 7
#define DRUMS_IN_PATCH 8
#define PATCH_COUNT 8



class Storage {

    public: 
        Storage(DaisyPatch *hw) {
            this->hw = hw;
            savedKit = new PersistentStorage<KitPatch>(hw->seed.qspi);
        }

        void Load(u8 patch, Runner::Kit *kit);
        void Save(u8 patch, Runner::Kit *kit);

    private: 
        struct DrumPatch {
            float params[PATCH_SIZE];

            bool operator!=(const DrumPatch& p) const {
                bool equal = true;
                for (u8 i = 0; i < PATCH_SIZE; i++) {
                    if (p.params[i] != params[i]) {
                        equal = false;
                    }
                }
                return !equal;
            }
        };

        struct KitPatch {
            u16 version = CURRENT_VERSION;
            DrumPatch drumPatches[DRUMS_IN_PATCH];

            bool operator!=(const KitPatch& p) const {
                bool equal = true;
                for (u8 i = 0; i < DRUMS_IN_PATCH; i++) {
                    if (p.drumPatches[i] != drumPatches[i]) {
                        equal = false;
                    }
                }
                return !equal;
            }
        };

        DaisyPatch *hw;
        PersistentStorage<KitPatch> *savedKit;


};

#endif
