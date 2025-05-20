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
        static const uint8_t PARAM_COUNT = 6;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_FREQUENCY = 0;
        static const uint8_t PARAM_FREQUENCY2 = 1;
        static const uint8_t PARAM_AMP_DECAY = 2;
        static const uint8_t PARAM_OSC2_LEVEL = 3;
        static const uint8_t PARAM_RINGMOD_LEVEL = 4;
        static const uint8_t PARAM_SQUEEZE = 5;
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
        std::string paramNames[PARAM_COUNT] = { "Frq1", "Frq2", "Dcy", "Lvl2", "rMod", "Sqz" };
        std::string slot;
        bool active = false;
        Param parameters[PARAM_COUNT];
        float velocity;

        // uses 808 tom design + 2nd oscillator
        // see https://www.baratatronix.com/blog/808-tom-synthesis
        Oscillator osc;
        Oscillator osc2;
        Svf oscFilter;
        WhiteNoise noise;
        OnePole noiseFilter;
        AdEnv ampEnv, pitchEnv, noiseEnv;

        static float presets[IDRUM_PRESET_COUNT][PARAM_COUNT];
        static float mtPresets[IDRUM_PRESET_COUNT][PARAM_COUNT];
        static float htPresets[IDRUM_PRESET_COUNT][PARAM_COUNT];

};

#endif
