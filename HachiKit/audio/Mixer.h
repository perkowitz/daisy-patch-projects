#ifndef MIXER_H
#define MIXER_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"
#include "Channel.h"

class Mixer {

    public:
        static const u8 CHANNELS = 16;

        // Number of settable mixer-global parameters.
        static const u8 PARAM_COUNT = 3;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_OUTPUT_GAIN = 0;
        static const uint8_t PARAM_SEND1_GAIN = 1;
        static const uint8_t PARAM_SEND2_GAIN = 2;
        u8 ParamCount() { return PARAM_COUNT; }

        Mixer() {
            globalParams[0].Init("Gain", 2, 0, 5, Parameter::EXPONENTIAL, 100);
            globalParams[1].Init("Fx1", 2, 0, 5, Parameter::EXPONENTIAL, 100);
            globalParams[2].Init("Fx2", 2, 0, 5, Parameter::EXPONENTIAL, 100);
            globalParamSet.Init(PARAM_COUNT, globalParams);
        }

        void Reset();
        void ResetSignals();
        void Process();

        void UpdateSignal(u8 channel, float signal);
        bool UpdateChannelParam(u8 channel, u8 param, float raw);
        bool UpdateChannelParam(u8 channel, u8 param, float raw, bool forceUpdate);
        void SetChannelParam(u8 channel, u8 param, float scaled);
        void ResetChannelParams(u8 channel);
        std::string GetChannelParamDisplay(u8 channel, u8 param);
        std::string GetChannelParamName(u8 channel, u8 param);

        float LeftSignal() { return leftSignal; }
        float RightSignal() { return rightSignal; }
        float Send1Signal() { return send1Signal; }
        float Send2Signal() { return send2Signal; }

        ParamSet* GetGlobals() { return &globalParamSet; }
        float OutputGain() { return globalParamSet.GetParamValue(PARAM_OUTPUT_GAIN); }
        float Send1Gain() { return globalParamSet.GetParamValue(PARAM_SEND1_GAIN); }
        float Send2Gain() { return globalParamSet.GetParamValue(PARAM_SEND2_GAIN); }


    private:
        static const uint8_t OUTPUT_LIMIT = 2;

        Channel channels[CHANNELS];
        float leftSignal = 0.0f;
        float rightSignal = 0.0f;
        float send1Signal = 0.0f;
        float send2Signal = 0.0f;

        ParamFull globalParams[PARAM_COUNT];
        ParamSet globalParamSet;


};


#endif