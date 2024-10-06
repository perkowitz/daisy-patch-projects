#include "ParamSet.h"

using namespace daisy;
using namespace daisysp;


u8 ParamSet::ParamCount() { 
    return paramCount; 
}

bool ParamSet::UpdateParam(u8 param, float raw) {
    return UpdateParam(param, raw, false);
}

bool ParamSet::UpdateParam(u8 param, float raw, bool forceUpdate) {
    return param < paramCount ? params[param].Update(raw, forceUpdate) : false;    
}

void ParamSet::SetParam(u8 param, float scaled) {
    if (param < paramCount) {
        params[param].SetScaledValue(scaled);
    }
}

void ParamSet::ResetParams() {
    for (u8 param = 0; param < paramCount; param++) {
        params[param].Reset();
    }
}

float ParamSet::GetParamValue(u8 param) {
    return param < paramCount ? params[param].Value() : 0.0f;
}

std::string ParamSet::GetParamDisplay(u8 param) {
    return param < paramCount ? params[param].Display() : "";    
}

std::string ParamSet::GetParamName(u8 param) {
    return param < paramCount ? params[param].Name() : "";    
}
