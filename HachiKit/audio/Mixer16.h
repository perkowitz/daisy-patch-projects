#ifndef MIXER16_H
#define MIXER16_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../utility.h"

struct SimpleChannel {
    float signal;
    float level; 
    float pan;
    float send1;
    float send2;
};

class Mixer16 {

    public:
        static const uint8_t CHANNELS = 16;

        void Reset();
        void ResetSignals();
        void Process();

        SimpleChannel* GetChannel(u8 channel);
        void UpdateSignal(u8 channel, float signal);
        void SetLevel(u8 channel, float level);
        void SetPan(u8 channel, float pan);
        void SetSend1(u8 channel, float level);
        void SetSend2(u8 channel, float level);

        float LeftSignal() { return leftSignal; }
        float RightSignal() { return rightSignal; }
        float Send1Signal() { return send1Signal; }
        float Send2Signal() { return send2Signal; }

        void SetOutputGain(float gain) { this->outputGain = gain; }
        void SetSend1Gain(float gain) { this->send1Gain = gain; }
        void SetSend2Gain(float gain) { this->send2Gain = gain; }

    private:
        static const uint8_t OUTPUT_LIMIT = 2;

        SimpleChannel channels[CHANNELS];
        float leftSignal = 0.0f;
        float rightSignal = 0.0f;
        float send1Signal = 0.0f;
        float send2Signal = 0.0f;

        float outputGain = 1;
        float send1Gain = 1;
        float send2Gain = 1;

};


#endif