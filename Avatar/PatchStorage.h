/**
 * PatchStorage manages saving and loading patches
 * 
*/
#ifndef PATCHSTORAGE_H
#define PATCHSTORAGE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../HachiLib/HachiLib.h"

using namespace daisy;
using namespace daisysp;

class PatchStorage {

    public:
        // Number of settable parameters.
        static const u8 PARAM_COUNT = 3;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_CURRENT_PATCH = 0;
        static const uint8_t PARAM_OPERATION = 1;
        static const uint8_t PARAM_TARGET_PATCH = 2;
        u8 ParamCount() { return PARAM_COUNT; }

        static const uint8_t OPERATION_NOP = 0;
        static const uint8_t OPERATION_LOAD = 1;
        static const uint8_t OPERATION_SAVE = 2;

        PatchStorage() {
            params[0].Init("Curr", 0, 0, 7, Parameter::LINEAR, 1); // should use PATCH_COUNT
            params[1].Init("Oper", 0, 0, 2, Parameter::LINEAR, 1);
            params[2].Init("Targ", 0, 0, 7, Parameter::LINEAR, 1);
            paramSet.Init(PARAM_COUNT, params);
        }

        std::string GetOperation() {
            u8 op = paramSet.GetParamValue(PARAM_OPERATION);
            switch (op) {
                case OPERATION_NOP:
                    return "---";
                case OPERATION_LOAD:
                    return "Load";
                case OPERATION_SAVE:
                    return "Save";
            }
            return "";
        }

        ParamSet* GetParamSet() { return &paramSet; }

    private:
        ParamFull params[PARAM_COUNT];
        ParamSet paramSet;
};


#endif
