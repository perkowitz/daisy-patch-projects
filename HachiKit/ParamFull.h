/**
 * Param tracks the value of a model parameter in response to a control (e.g. analog pot)
 * - ignores small changes to avoid jitter.
 * - doesn't update the stored value until the input has been swept past that value, to avoid jumps.
 * 
 * Raw value is between 0 and 1. Scaled value can be in any range; Param does not compute the
 * scaling, so must be managed externally.
 * 
*/
#ifndef PARAMFULL_H
#define PARAMFULL_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "Utility.h"

using namespace daisy;
using namespace daisysp;

class ParamFull {

    public:

        ParamFull() {
            this->name = "";
            Reset();
        }

        ParamFull(std::string name) {
            this->name = name;
            Reset();
        }

        ParamFull(std::string name, float initValue, float scaleLo, float scaleHi, Parameter::Curve scaleCurve, u16 displayScale) {
            Init(name, initValue, scaleLo, scaleHi, scaleCurve, displayScale);
            Reset();
        }

        void Init(std::string name, float initValue, float scaleLo, float scaleHi, Parameter::Curve scaleCurve, u16 displayScale) {
            this->name = name;
            this->scaled = initValue;
            this->scaleLo = scaleLo;
            this->scaleHi = scaleHi;
            this->scaleCurve = scaleCurve;
        }

        std::string Name() { return name; }
        float Value() { return scaled; }

        /**
         * Reset sets the Param back to a state where it is waiting for control.
        */
        void Reset() {
            raw = -1.0f;   // any value (0-1) will appear as a change
            active = lower = higher = false;
        }

        /**
         * SetScaledValue stores the value directly and overrides the jitter and jump checking.
        */
        void SetScaledValue(float scaled) {
            this->scaled = scaled;
            Reset();
        }

        /**
         * Update checks the raw value for jitter and prevents jump in value.
         * 
         * Returns true if the value changed.
        */
        bool Update(float raw) {
            float scaled = Utility::ScaleFloat(raw, scaleLo, scaleHi, scaleCurve);
            if (!active) {
                if (scaled <= this->scaled || (raw <= 0.01 && raw >= 0.0f)) lower = true;
                if (scaled >= this->scaled || raw >= 0.99) higher = true;
                active = (lower && higher);
            }
            if (active && std::abs(raw - this->raw) > delta) {
                this->raw = raw;
                this->scaled = scaled;
                return true;
            }
            return false;
        }

        std::string Display() {
            return std::to_string((int)(scaled * displayScale));
        }

    private:
        std::string name;
        float delta = 0.001f;  // TODO: make static?
        float raw = 0.0f;
        float scaled = 0.0f;
        bool active = false;
        bool lower = false;
        bool higher = false;
        float scaleLo = 0;
        float scaleHi = 1;
        u16 displayScale = 1;
        Parameter::Curve scaleCurve = Parameter::LINEAR;

};

typedef ParamFull* ParamPtr;

#endif

