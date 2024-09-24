#ifndef RUNNER_H
#define RUNNER_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "util/CpuLoadMeter.h"
#include <string>
#include "Utility.h"
#include "audio/Mixer.h"
#include "audio/Processing.h"
#include "DrumWrapper.h"
#include "IDrum.h"
#include "Screen.h"

using namespace daisy;
using namespace daisysp;


#define MINIMUM_NOTE 36
#define KNOB_COUNT 4
#define CPU_SINGLE false
#define MENU_ROWS 2
#define MENU_SOUNDS 0
#define MENU_MIXER 1


class Runner {

    public:

        struct Kit {
            u8 drumCount;
            IDrum** drums;
            u8 sourceCount;
            IDrum** sources;
        };

        Runner(SaiHandle::Config::SampleRate audioSampleRate) {
            hw.Init(true);                              // "true" boosts processor speed from 400mhz to 480mhz
            hw.SetAudioSampleRate(audioSampleRate);     // 8,16,32,48; higher rate requires more CPU
            samplerate = hw.AudioSampleRate();
            meter.Init(samplerate, 128, 1.0f);
            screen.setDisplay(&hw.display);
        }

        void Run(Kit *kit);
        float getSampleRate() { return samplerate; }

        static Runner *globalRunner;
        static void GlobalAudioCallback(AudioHandle::InputBuffer  in,
                AudioHandle::OutputBuffer out,
                size_t size);


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
        Screen screen;
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


};

#endif