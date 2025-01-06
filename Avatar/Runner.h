#ifndef RUNNER_H
#define RUNNER_H

#include "daisy_patch.h"
#include "daisysp.h"
#include "util/CpuLoadMeter.h"
#include <string>
#include "lib/audio/Mixer.h"
#include "Screen.h"
#include "ISynth.h"

using namespace daisy;
using namespace daisysp;


#define MINIMUM_NOTE 36
#define KNOB_COUNT 4
#define CPU_SINGLE false
#define MENU_ROWS 2
#define MENU_SYNTH 0
#define MENU_MIXER 1
#define MENU_MIXER_MAIN 2
#define MENU_PATCH 3
#define MIDIMAP_SIZE 16
#define AUDIO_PASSTHRU false
#define SHOW_CPU false

#define CURRENT_VERSION 0
#define PATCH_SIZE 7
#define DRUMS_IN_PATCH 8
#define PATCH_COUNT 8
#define START_PROGRAM_CHANGE 0

#define LONG_PRESS_MILLIS 2000
#define SCREEN_SAVE_MILLIS 60000
#define UPDATE_CLOCK_TICKS 512

#define STEPS_PER_MEASURE 16
#define TICKS_PER_STEP 6
#define MIDICC_LIMIT 16
#define MIDICC_VOLUME 16
#define MIDICC_SEND_1 32
#define MIDICC_PARAM_1 48
#define MIDICC_PARAM_2 64
#define MIDICC_PARAM_3 80
#define MIDICC_PARAM_4 96


class Runner {
    public:

        Runner(SaiHandle::Config::SampleRate audioSampleRate) {
            hw.Init(true);                              // "true" boosts processor speed from 400mhz to 480mhz
            hw.SetAudioSampleRate(audioSampleRate);     // 8,16,32,48; higher rate requires more CPU
            samplerate = hw.AudioSampleRate();
            meter.Init(samplerate, 128, 1.0f);
            screen.setDisplay(&hw.display);
            screen.DrawLine(0, 0, 60, 60, true);
        }

        void Run(ISynth *synth1, ISynth *synth2);
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
        void MidiSend(MidiEvent m);
        void HandleMidiRealtime(MidiEvent m);
        void HandleMidiNote(ISynth *synth, u8 note, u8 velocity);
        void HandleMidiMessage(MidiEvent m);

        float samplerate = 0;

        DaisyPatch hw;
        Screen screen;
        CpuLoadMeter meter;
        // MidiUsbHandler usbMidi;

        ISynth *synth1;
        ISynth *synth2;
        ISynth *currentSynth;
        u8 synth1Page = 0;
        u8 synth2Page = 0;
        u8 currentSynthPage = 0;

        u8 *midiChannels;
        Mixer mixer;
        u8 mixerSections = 1;

        u8 currentMenu = 0; 
        u8 currentMenuIndex = 0;
        u8 currentMixerSection = 0;

        bool running = false;
        u8 currentMeasure = 0;
        u8 currentStep = 0;
        u8 currentTick = 0;

        float lastKnobValue[KNOB_COUNT];

        u8 midiChannel = 4;  // 0-indexed
        u8 midiCcOffset = 0;

        u8 menuSize = Screen::MENU_SIZE;

        u8 cycle = 0;
        u8 cycleLength = 8;
        float savedSignal = 0.0f;

        u8 clockRange = 8;
        u8 clockThreshold = 8;
        float mainGain = 1;

        u32 lastEncoderTime = 0;
        u32 lastScreenTime = 0;
};

#endif
