#include "Storage.h"

using namespace daisy;
using namespace daisysp;


void Storage::Load(u8 patch, Runner::Kit *kit) {

	KitPatch &kitPatch = savedKit->GetSettings();

    for (u8 d = 0; d < std::min((u8)DRUMS_IN_PATCH, kit->drumCount); d++) {
        if (kit->drums[d] != nullptr) {
            IDrum *drum = kit->drums[d];
            for (u8 p = 0; p < std::min((u8)PATCH_SIZE, drum->ParamCount()); p++) {
                drum->SetParam(p, kitPatch.drumPatches[d].params[p]);
            }
        }        
    }

}

void Storage::Save(u8 patch, Runner::Kit *kit) {

	KitPatch &kitPatch = savedKit->GetSettings();

    for (u8 d = 0; d < std::min((u8)DRUMS_IN_PATCH, kit->drumCount); d++) {
        if (kit->drums[d] != nullptr) {
            IDrum *drum = kit->drums[d];
            for (u8 p = 0; p < std::min((u8)PATCH_SIZE, drum->ParamCount()); p++) {
                kitPatch.drumPatches[d].params[p] = drum->GetParam(p);
            }
        }        
    }

}
