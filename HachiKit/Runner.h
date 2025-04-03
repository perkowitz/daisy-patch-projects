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
#include "PatchStorage.h"
#include "Screen.h"

using namespace daisy;
using namespace daisysp;


#define MINIMUM_NOTE 36
#define KNOB_COUNT 4
#define CPU_SINGLE false
#define MENU_ROWS 2
#define MENU_SOUNDS 0
#define MENU_MIXER 1
#define MENU_MIXER_MAIN 2
#define MENU_PATCH 3
#define MIDIMAP_SIZE 16
#define AUDIO_PASSTHRU true
#define SHOW_CPU false

#define CV_OUT_COUNT 2
#define CV_OUT_SCALE_FACTOR 4095

#define CURRENT_VERSION 0
#define PATCH_SIZE 7
#define DRUMS_IN_PATCH 8
#define PATCH_COUNT 8
#define START_PROGRAM_CHANGE 0

#define LONG_PRESS_MILLIS 2000
#define SCREEN_SAVE_MILLIS 60000
#define UPDATE_CLOCK_TICKS 512
#define TRIGGER_ALL_SOURCE -1

#define MIDICC_LIMIT 16
#define MIDICC_VOLUME 16
#define MIDICC_SEND_1 32
#define MIDICC_PARAM_1 48
#define MIDICC_PARAM_2 64
#define MIDICC_PARAM_3 80
#define MIDICC_PARAM_4 96


class Runner {

    public:
        struct Kit {
            u8 drumCount;
            IDrum** drums;
            u8 sourceCount;
            IDrum** sources;
            IDrum** midiMap;
        };

        struct DrumPatch {
            float params[PATCH_SIZE];

            bool operator!=(const DrumPatch& p) const {
                bool equal = true;
                for (u8 i = 0; i < PATCH_SIZE; i++) {
                    if (p.params[i] != params[i]) {
                        equal = false;
                    }
                }
                return !equal;
            }
        };

        struct KitPatch {
            u16 version = CURRENT_VERSION;
            DrumPatch drumPatches[DRUMS_IN_PATCH];

            bool operator!=(const KitPatch& p) const {
                bool equal = true;
                for (u8 i = 0; i < DRUMS_IN_PATCH; i++) {
                    if (p.drumPatches[i] != drumPatches[i]) {
                        equal = false;
                    }
                }
                return !equal;
            }
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
        void UpdateFromEncoder();
        void ProcessEncoder();
        void ProcessKnobs();
        void ProcessControls();
        void AudioCallback(AudioHandle::InputBuffer  in,
                AudioHandle::OutputBuffer out,
                size_t size);
        void MidiSend(MidiEvent m);
        void HandleMidiMessage(MidiEvent m);
        void Load(u8 patch, Runner::Kit *kit, PersistentStorage<KitPatch> *savedKit);
        void SaveToKitPatch(Runner::Kit *kit, Runner::KitPatch *kitPatch);
        void Save(u8 patch, Runner::Kit *kit, PersistentStorage<KitPatch> *savedKit);
        void CvOut(u8 output, float value);
        void GateOut(bool value);

        float samplerate = 0;

        DaisyPatch hw;
        Screen screen;
        CpuLoadMeter meter;
        // MidiUsbHandler usbMidi;
        Mixer mixer;

        Kit *kit;
        u8 mixerSections = 4;

        u8 currentMenu = 0; 
        u8 currentMenuIndex = 0;
        u8 newMenuIndex = 0;
        bool redrawScreen = false;
        bool turnScreenOn = false;   
        uint8_t currentDrum = 0;
        uint8_t currentKnobRow = 0;
        u8 currentKnob = 0;
        u8 currentMixerSection = 0;
        u8 maxDrum = 1;
        float lastKnobValue[KNOB_COUNT];
        u8 midiChannel = 9;  // 0-indexed
        u8 midiCcOffset = 0;

        u8 currentPatch = 0;
        PatchStorage patchStorage;
        u8 menuSize = Screen::MENU_SIZE;

        u8 cycle = 0;
        u8 cycleLength = 8;
        float savedSignal = 0.0f;

        u8 clockRange = 8;
        u8 clockThreshold = 8;
        float mainGain = 1;

        s8 gateOutSource = TRIGGER_ALL_SOURCE;
        u8 gateOutLengthMillis = 40;
        u32 lastGateOutTime = 0;

        PersistentStorage<KitPatch> *savedKits[PATCH_COUNT];
        s8 saveTo = -1;
        s8 loadFrom = -1;
        u32 lastEncoderTime = 0;
        u32 lastScreenTime = 0;


};

#endif
