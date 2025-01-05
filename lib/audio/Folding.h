#ifndef FOLDING_H
#define FOLDING_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "../Utility.h"

using namespace daisy;
using namespace daisysp;

class Folding {

    public:
        
        static float Squeeze(float signal, float intensity) {
            float scale = 1 + intensity;
            return Utility::LimitFloat(signal * scale, -1, 1);
        }

        static float Wrap(float signal, float intensity) {
            float scale = 1 + intensity;
            float s = signal * scale;
            while (s < -1 || s > 1) {
                s = s > 1 ? s - 2 : s + 2;
            }
            return s;
        }

        static float Fold(float signal, float intensity) {
            float scale = 1 + intensity;
            float s = signal * scale;
            while (s < -1 || s > 1) {
                s = s > 1 ? 2 - s : -2 - s;
            }
            return s;
        }

};

#endif
