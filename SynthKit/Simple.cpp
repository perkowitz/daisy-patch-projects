#include "Simple.h"

using namespace daisy;
using namespace daisysp;

float Simple::Process() {
    float signal = osc.Process();
    svf.Process(signal);
    signal = svf.Low();
    leftSignal = velocity * signal * ampEnv.Process();
    rightSignal = leftSignal;
    active = ampEnv.IsRunning();
    return leftSignal;
}

float Simple::GetOutput(u8 channel) {
    return channel == 0 ? leftSignal : rightSignal;
}

void Simple::NoteOn(u8 note, float velocity) {
    this->velocity = Utility::Limit(velocity);
    if (this->velocity == 0) {
        NoteOff(note);
        return;
    }

    osc.SetFreq(mtof(note));
    osc.Reset();
    ampEnv.GateOn();
    // if (retrigger || activeGates == 0) {
    //     ampEnv.Retrigger(false);
    // }
    activeGates++;
    active = true;
}

void Simple::NoteOff(u8 note) {
    // if (activeGates > 0) {
    //     activeGates--;
    // }
    ampEnv.GateOff();
}

ParamSet *Simple::GetParamSet(u8 page) {
    if (page < PAGE_COUNT) {
        return &pages[page];
    }
    return nullptr;
}

std::string Simple::GetPageName(u8 page) {
    if (page < PAGE_COUNT) {
        return pageNames[page];
    }
    return "";
}

void Simple::ResetParams(u8 page) {
    if (page < PAGE_COUNT) {
        pages[page].ResetParams();
    }
}
