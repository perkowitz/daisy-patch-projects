#ifndef DRIFT_H
#define DRIFT_H

#include <ctime>
#include "daisy_patch.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define DRIFT_STEPS 8

class Drift {

    public:
        void Init() {  
            srand (static_cast <unsigned> (time(0)));
            Randomize(true);
            Randomize(false);
        }

        // Set rate of change; higher = slower.
        void SetRate(u8 rate) { 
            this->rate = rate; 
        }

        void SetLoopLength(u8 loopLength) {
            if (loopLength > 0 && loopLength <= DRIFT_STEPS) {
                this->loopLength = loopLength;
            }
        }

        // Return default signal value.
        float Signal() { 
            return Signal(0);
        }

        // Return signal value offset to other steps, to produce distinct modulations.
        float Signal(s8 offset) {
            u8 step = (currentStep + offset) % DRIFT_STEPS;
            return currentValues[step];
        }

        // Trigger a new step.
        void Trigger() { 
            currentStep = (currentStep + 1) % loopLength;
            UpdateStep(currentStep);
            if (currentStep == 0) {
                if (rateCountdown == 0) {
                    //Randomize();  // steps do this during update
                    rateCountdown = rate;  // if rate==0 will just randomize every time
                } else {
                    rateCountdown--;
                }
            } 
        }

        // Return to the first step.
        void Reset() {
            currentStep = 0;
            UpdateStep(currentStep);
        }

        // Jump to all target values and randomize new values.
        void Jump() {
            for (u8 i = 0; i < DRIFT_STEPS; i++) {                
                currentValues[i] = targetValues[i];
            }
            Randomize();
        }

        // Randomize target values.
        void Randomize() { Randomize(true); }

        // Randomize current or target values.
        void Randomize(bool shouldRandomizeTargets) {
            for (u8 i = 0; i < DRIFT_STEPS; i++) {                
                if (shouldRandomizeTargets) {
                    targetValues[i] = NewValue();
                } else {
                    currentValues[i] = NewValue();
                }
            }
        }

    private:
        float NewValue() {
            return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        }

        void UpdateStep(u8 step) {
            if (step < DRIFT_STEPS) {
                if (rateCountdown == 0) {
                    currentValues[step] = targetValues[step];
                    targetValues[step] = NewValue();
                } else {
                    float increment = (targetValues[step] - currentValues[step]) / rateCountdown;
                    currentValues[step] += increment;
                }
            }
        }

        u8 loopLength = DRIFT_STEPS;
        float currentValues[DRIFT_STEPS];
        float targetValues[DRIFT_STEPS];
        u8 rate;
        u8 rateCountdown;
        u8 currentStep;

        // std::default_random_engine generator;
        // std::uniform_real_distribution<float> distribution(0, 1);

};



#endif
