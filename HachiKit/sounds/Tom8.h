#ifndef TOM8_H
#define TOM8_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "../IDrum.h"
#include "../Utility.h"
#include "../Param.h"

using namespace daisy;
using namespace daisysp;

class Tom8: public IDrum {

    public:
        // Number of settable parameters for this model.
        static const uint8_t PARAM_COUNT = 2;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_FREQUENCY = 0;
        static const uint8_t PARAM_AMP_DECAY = 1;
        u8 ParamCount() { return PARAM_COUNT; }

        void Init(std::string slot, float sample_rate);
        void Init(std::string slot, float sample_rate, float frequency);
        float Process();
        void Trigger(float velocity);
        bool IsActive() { return active; }

        float GetParam(uint8_t param);
        float UpdateParam(uint8_t param, float value);
        void SetParam(uint8_t param, float value);
        void ResetParams();
        std::string GetParamString(uint8_t param);

        std::string Name() { return "Tom"; }
        std::string Slot() { return slot; }
        std::string GetParamName(uint8_t param) { return param < PARAM_COUNT ? paramNames[param] : ""; }

        void LoadPreset(u8 preset);

    private:
        std::string paramNames[PARAM_COUNT] = { "Freq", "Dcy" };
        std::string slot;
        bool active = false;
        Param parameters[PARAM_COUNT];
        float velocity;

        // see https://www.baratatronix.com/blog/808-tom-synthesis
        Oscillator osc;
        Svf oscFilter;
        WhiteNoise noise;
        OnePole noiseFilter;
        AdEnv ampEnv, pitchEnv, noiseEnv;

        static float presets[IDRUM_PRESET_COUNT][PARAM_COUNT];

};

#endif
