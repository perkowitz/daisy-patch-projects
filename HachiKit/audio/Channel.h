/**
 * Channel represents a single channel of a mixer.
 * 
*/
#ifndef CHANNEL_H
#define CHANNEL_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"
#include "../ParamFull.h"
#include "../ParamSet.h"

using namespace daisy;
using namespace daisysp;

class Channel {

    public:
        // Number of settable parameters.
        static const u8 PARAM_COUNT = 4;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_LEVEL = 0;
        static const uint8_t PARAM_PAN = 1;
        static const uint8_t PARAM_SEND1 = 2;
        static const uint8_t PARAM_SEND2 = 3;
        u8 ParamCount() { return PARAM_COUNT; }

        Channel() {
            params[0].Init("Lvl", 1, 0, 1, Parameter::EXPONENTIAL, 100);
            params[1].Init("Pan", 0, -1, 1, Parameter::LINEAR, 1);
            params[2].Init("A", 0, 0, 1, Parameter::EXPONENTIAL, 100);
            params[3].Init("B", 0, 0, 1, Parameter::EXPONENTIAL, 100);
            paramSet.Init(PARAM_COUNT, params);
        }

        ParamSet* GetParamSet() { return &paramSet; }
        float Signal() { return signal; }
        void SetSignal(float signal) { this->signal = signal; }

        // Shortcuts for GetParamValue.
        float Level() { return paramSet.GetParamValue(PARAM_LEVEL); }
        float Pan() { return paramSet.GetParamValue(PARAM_PAN); }
        float Send1() { return paramSet.GetParamValue(PARAM_SEND1); }
        float Send2() { return paramSet.GetParamValue(PARAM_SEND2); }

    private:
        float signal;
        ParamFull params[PARAM_COUNT];
        ParamSet paramSet;
};


#endif
