#ifndef CY_H
#define CY_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "../IDrum.h"
#include "../ISource.h"
#include "../Utility.h"
#include "../Param.h"
#include "HhSource68.h"

using namespace daisy;
using namespace daisysp;

class Cy: public IDrum {

    public:
        // Number of settable parameters for this model.
        static const uint8_t PARAM_COUNT = 4;
        // This is the order params will appear in the UI.
        static const uint8_t PARAM_ATTACK = 0;
        static const uint8_t PARAM_DECAY = 1;
        static const uint8_t PARAM_HPF = 2;
        static const uint8_t PARAM_LPF = 3;
        u8 ParamCount() { return PARAM_COUNT; }

        static const float HPF_MAX;
        static const float HPF_MIN;
        static const float LPF_MAX;
        static const float LPF_MIN;
        // static const u16 BUFFER_SIZE = 100;

        void Init(std::string slot, float sample_rate);
        void Init(std::string slot, float sample_rate, float attack, float decay, HhSource68 *source, float hpfCutoff, float lpfCutoff);
        float Process();
        void Trigger(float velocity);
        bool IsActive() { return active; }

        float GetParam(uint8_t param);
        float UpdateParam(uint8_t param, float value);
        void SetParam(uint8_t param, float value);
        void ResetParams();
        std::string GetParamString(uint8_t param);

        std::string Name() { return "Cy"; }
        std::string Slot() { return slot; }
        std::string GetParamName(uint8_t param) { return param < PARAM_COUNT ? paramNames[param] : ""; }

        void LoadPreset(u8 preset);

    private:
        std::string paramNames[PARAM_COUNT] = { "Atk", "Dcy", "Hpf", "Lpf" };
        std::string slot;
        bool active = false;
        Param parameters[PARAM_COUNT];
        float velocity;
        // ISource *source = NULL;
        HhSource68 *source = NULL;
        AdEnv env;
        Svf hpf, lpf;

        // float buffer[BUFFER_SIZE];
        // bool bufferValid = false;
        // u16 bufferIndex = 0;

        static float presets[IDRUM_PRESET_COUNT][PARAM_COUNT];

};



#endif
