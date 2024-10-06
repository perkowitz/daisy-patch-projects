#ifndef MIXER_H
#define MIXER_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"
#include "Channel.h"

class Mixer {

    public:
        static const u8 CHANNELS = 16;

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

        // TODO: use params
        // void SetOutputGain(float gain) { this->outputGain = gain; }
        // void SetSend1Gain(float gain) { this->send1Gain = gain; }
        // void SetSend2Gain(float gain) { this->send2Gain = gain; }

    private:
        static const uint8_t OUTPUT_LIMIT = 2;

        Channel channels[CHANNELS];
        float leftSignal = 0.0f;
        float rightSignal = 0.0f;
        float send1Signal = 0.0f;
        float send2Signal = 0.0f;

        float outputGain = 1;
        float send1Gain = 1;
        float send2Gain = 1;

};


#endif