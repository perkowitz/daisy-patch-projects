#ifndef PARAMSET_H
#define PARAMSET_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "Utility.h"
#include "ParamFull.h"

using namespace daisy;
using namespace daisysp;

class ParamSet {

    public:

        ParamSet() {
            paramCount = 0;
            params = nullptr;
        }

        ParamSet(u8 paramCount, ParamFull* params) {
            this->paramCount = paramCount;
            this->params = params;
        }

        void Init(u8 paramCount, ParamFull* params) {
            this->paramCount = paramCount;
            this->params = params;
        }

        u8 ParamCount();
        bool UpdateParam(u8 param, float raw);
        void SetParam(u8 param, float scaled);
        void ResetParams();
        float GetParamValue(u8 param);
        std::string GetParamDisplay(u8 param);
        std::string GetParamName(u8 param);

    private:
        u8 paramCount;
        ParamFull* params;

};



#endif
