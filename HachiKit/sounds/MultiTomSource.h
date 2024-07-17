#ifndef MULTITOMSOURCE_H
#define MULTITOMSOURCE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "../IDrum.h"
#include "../ISource.h"
#include "../Utility.h"
#include "../Param.h"
#include "ClickSource.h"

using namespace daisy;
using namespace daisysp;

class MultiTomSource: public IDrum {

    public:
        // Number of settable parameters for this model.
        static const uint8_t PARAM_COUNT = 1;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_AMP_DECAY = 0;
        u8 ParamCount() { return PARAM_COUNT; }

        void Init(std::string slot, float sample_rate);
        void Init(std::string slot, float sample_rate, float frequency, ClickSource *clickSource);
        float Process();
        float Signal() { return signal; }
        void Trigger(float velocity);
        void Trigger(float velocity, u8 index, float frequency);
        bool IsActive() { return active; }

        float GetParam(uint8_t param);
        float UpdateParam(uint8_t param, float value);
        void SetParam(uint8_t param, float value);
        void ResetParams();
        std::string GetParamString(uint8_t param);

        std::string Name() { return "Tom"; }
        std::string Slot() { return slot; }
        std::string GetParamName(uint8_t param) { return param < PARAM_COUNT ? paramNames[param] : ""; }

    private:
        std::string paramNames[PARAM_COUNT] = { "Freq" };
        std::string slot;
        bool active = false;
        Param parameters[PARAM_COUNT];

        float velocity;
        float currentBaseFrequency = 0.0f;
        float currentGain = 1.0f;
        u8 currentIndex = 0;
        float retriggerThreshold = 0.1f;
        float retriggerGainScale = 1.1f;
        // float retriggerFreqScale = 1.01f;
        float retriggerFreqScale = 1.0f;
        float ampEnvSignal = 0.0f;
        float signal = 0.0f;

        Oscillator osc;
        AdEnv ampEnv, pitchEnv;
        ClickSource *clickSource;

        float SetParam(uint8_t param, float value, bool isRaw);

};



#endif
