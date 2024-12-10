#include "ParamPage.h"

using namespace daisy;
using namespace daisysp;


bool ParamPage::UpdateParam(u8 param, float raw) {
    return UpdateParam(param, raw, false);
}

bool ParamPage::UpdateParam(u8 param, float raw, bool forceUpdate) {
    return param < paramCount ? params[param]->Update(raw, forceUpdate) : false;    
}

void ParamPage::SetParam(u8 param, float scaled) {
    if (param < paramCount) {
        params[param]->SetScaledValue(scaled);
    }
}

void ParamPage::ResetParams() {
    for (u8 param = 0; param < paramCount; param++) {
        params[param]->Reset();
    }
}

float ParamPage::GetParamValue(u8 param) {
    return param < paramCount ? params[param]->Value() : 0.0f;
}

std::string ParamPage::GetParamDisplay(u8 param) {
    return param < paramCount ? params[param]->Display() : "";    
}

std::string ParamPage::GetParamName(u8 param) {
    return param < paramCount ? params[param]->Name() : "";    
}
