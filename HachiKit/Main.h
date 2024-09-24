#ifndef MAIN_H
#define MAIN_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "util/CpuLoadMeter.h"
#include <string>
#include "Utility.h"
#include "audio/Mixer.h"
#include "audio/Processing.h"
#include "DrumWrapper.h"
#include "IDrum.h"
#include "Kits.h"
#include "Screen.h"

using namespace daisy;
using namespace daisysp;


#define MINIMUM_NOTE 36
#define KNOB_COUNT 4
#define CPU_SINGLE false
#define MENU_ROWS 2
#define MENU_SOUNDS 0
#define MENU_MIXER 1


class Main {

    public:

        struct Kit {
            u8 drumCount;
            IDrum* drums;
            u8 sourceCount;
            IDrum* sources;
        };

        float InitHardware(SaiHandle::Config::SampleRate audioSampleRate);
        void Run(Kit *kit);

    private:
        void DisplayParamMenu();
        void DisplayKnobValues();
        void DrawScreen(bool clearFirst);
        void ProcessEncoder();
        void ProcessKnobs();
        void ProcessControls();
        void AudioCallback(AudioHandle::InputBuffer  in,
                AudioHandle::OutputBuffer out,
                size_t size);
        void HandleMidiMessage(MidiEvent m);

        float samplerate = 0;

        DaisyPatch hw;
        Screen screen(&hw.display);
        CpuLoadMeter meter;
        // MidiUsbHandler usbMidi;
        Mixer mixer;

        Kit *kit;

        u8 currentMenu = 0; 
        u8 currentMenuIndex = 0;
        uint8_t currentDrum = 0;
        uint8_t currentKnobRow = 0;
        u8 currentMixerSection = 0;
        u8 maxDrum = 1;
        float lastKnobValue[KNOB_COUNT];

        u8 cycle = 0;
        u8 cycleLength = 8;
        float savedSignal = 0.0f;
        u32 usageCounter = 0;

        u8 clockRange = 8;
        u8 clockThreshold = 8;
        float mainGain = 1;


}

#endif
