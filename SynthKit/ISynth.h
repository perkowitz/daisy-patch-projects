#ifndef ISYNTH_H
#define ISYNTH_H

using namespace daisy;
using namespace daisysp;

#include "daisy_patch.h"
#include "daisysp.h"
#include "lib/ParamSet.h"
#include <string>


class ISynth {

    public:

        virtual ~ISynth() { }

        virtual std::string Name() = 0;
        virtual void Init(float sample_rate) = 0;
        virtual bool IsActive() = 0;

        virtual float Process() = 0;
        virtual float GetOutput(u8 channel) = 0;
        virtual void NoteOn(u8 note, float velocity) = 0;
        virtual void NoteOff(u8 note) = 0;

        virtual u8 PageCount() = 0;
        virtual ParamSet *GetParamSet(u8 page) = 0;
        virtual std::string GetPageName(u8 page) = 0;
        virtual void ResetParams(u8 page) = 0;

};


#endif
