#ifndef DRUMWRAPPER_H
#define DRUMWRAPPER_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "IDrum.h"
#include "Utility.h"

using namespace daisy;
using namespace daisysp;

#define CYCLECOUNT_THRESHOLD 1000

class DrumWrapper: public IDrum {

    public:
        u8 ParamCount() { return drum->ParamCount(); }

        void Init(std::string slot, float sample_rate) { drum->Init(slot, sample_rate); }
        void Init(IDrum *drum) { this->drum = drum; }
        float Process();
        void Trigger(float velocity);
        bool IsActive() { return drum->IsActive(); }

        float GetParam(uint8_t param) { return drum->GetParam(param); }
        std::string GetParamString(uint8_t param) { return drum->GetParamString(param); }
        float UpdateParam(uint8_t param, float value) { return drum->UpdateParam(param, value); }
        void SetParam(uint8_t param, float value) { drum->SetParam(param, value); }
        void ResetParams() { drum->ResetParams(); }

        std::string Name() { return drum->Name(); }
        std::string Slot() { return drum->Slot(); }
        std::string GetParamName(uint8_t param) { return drum->GetParamName(param); }

    private:
        IDrum *drum;
        u32 cycleCount = 0;

};



#endif
