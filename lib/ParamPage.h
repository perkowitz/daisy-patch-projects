#ifndef PARAMPAGE_H
#define PARAMPAGE_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "Utility.h"
#include "Param.h"

using namespace daisy;
using namespace daisysp;

class ParamPage {

    public:

        void Init(std::string moduleName, std::string pageTitle, Param *param0, Param *param1, Param *param2, Param *param3) {
            this->moduleName = moduleName;
            this->pageTitle = pageTitle;
            params[0] = param0;
            params[1] = param1;
            params[2] = param2;
            params[3] = param3;
            paramCount = 0;
            while (params[paramCount] != nullptr && paramCount < 4) {
                paramCount++;
            }
            // paramSet.Init(paramCount, params[0]);
        }

        u8 ParamCount() { return paramCount; }
        std::string getModuleName() { return moduleName; }
        std::string getTitle() { return pageTitle; }
        // ParamSet *GetParamSet() { return &paramSet; }

        bool UpdateParam(u8 param, float raw);
        bool UpdateParam(u8 param, float raw, bool forceUpdate);
        void SetParam(u8 param, float scaled);
        void ResetParams();
        float GetParamValue(u8 param);
        std::string GetParamDisplay(u8 param);
        std::string GetParamName(u8 param);

    private:
        u8 paramCount = 0;
        std::string moduleName = "";
        std::string pageTitle = "";
        Param *params[4];
        // ParamSet paramSet;

};



#endif
