#include <chrono>
#include <thread>

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

DaisyPatch hw;
Screen screen(&hw.display);
CpuLoadMeter meter;

Mixer mixer;

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





// Display the available parameter names.
void DisplayParamMenu() {

    screen.DrawRect(0, 9, 127, 36, false, true);

    // TODO make this work for mixer channels
    u8 knobRow = currentKnobRow;
    // if (knobRow * KNOB_COUNT >= drums[currentDrum]->ParamCount()) {
    //     knobRow = 0;
    // }

    std::string sc = "";
    bool selected = false;
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        // for (u8 row = 0; row <= drums[currentDrum]->PARAM_COUNT / 4; row++) {
        for (u8 row = 0; row < MENU_ROWS; row++) {
            Rectangle rect2(knob * 32, (row + 1) * 12, 32, 12);
            if (currentMenu == MENU_SOUNDS) {
                u8 param = row * KNOB_COUNT + knob;
                sc = drums[currentDrum]->GetParamName(param);
                selected = row == knobRow;
            } else if (currentMenu == MENU_MIXER) {
                u8 channel = currentMixerSection * 4 + knob;
                if (row == 0) {
                    selected = false;
                    sc = drums[channel]->Slot();  // show channel names on first row
                } else if (row == 1) {
                    sc = mixer.GetChannelParamName(channel, knob);
                    selected = knobRow == knob;   // for mixer, we show the selections on the 2nd row
                }
            }
            screen.DrawButton(rect2, sc, selected, selected, !selected);
            // hw.display.WriteStringAligned(sc.c_str(), Font_6x8, rect2, Alignment::centered, true);
            // hw.display.SetCursor(rect2.GetX(), rect2.GetY());
            // hw.display.WriteString(sc.c_str(), Font_6x8, true);
            // hw.display.DrawLine(0, rect2.GetY() + 11, 127, rect2.GetY() + 11, true);
        }
    }
}

// Display the current values and parameter names of model params for 4 knobs.
// Knob number == param number, since model params are listed in UI order.
void DisplayKnobValues() {

    screen.DrawRect(0, 0, 127, 11, false, true);

    u8 knobRow = currentKnobRow;
    // make this work for mixer rows
    // if (knobRow * KNOB_COUNT >= drums[currentDrum]->ParamCount()) {
    //     knobRow = 0;
    // }

    u8 index;
    std::string sc = "";
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SOUNDS) {
            index = knobRow * KNOB_COUNT + knob;
            sc = drums[currentDrum]->GetParamString(index);
        } else if (currentMenu == MENU_MIXER) {
            index = currentMixerSection * 4 + knob;
            sc = mixer.GetChannelParamDisplay(index, knobRow);
        }
        screen.WriteStringAligned(sc.c_str(), Font_6x8, rect, Alignment::centered, true);
        // screen.DrawButton(rect, sc, false, true, false);
    }
}

void DrawScreen(bool clearFirst) {
    if (clearFirst) {
        hw.display.Fill(false);
    }
    screen.DrawMenu(currentMenuIndex);
    DisplayParamMenu();
    DisplayKnobValues();
    hw.display.Update();        
}

void ProcessEncoder() {

    bool redraw = false;
    bool screenOn = false;

    int inc = hw.encoder.Increment();
    if (inc != 0) {
        screenOn = true;
    }
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, Screen::MENU_SIZE-1);
    // u8 newMenuIndex = (currentMenuIndex + inc) % Screen::MENU_SIZE;
    if (newMenuIndex != currentMenuIndex) {
        if (newMenuIndex < drumCount) {
            currentMenu = MENU_SOUNDS;
            u8 newDrum = newMenuIndex;
            drums[newDrum]->ResetParams();
            currentDrum = newDrum;
        } else {
            currentMenu = MENU_MIXER;
            currentMixerSection = newMenuIndex - drumCount;
            for (u8 i = 0; i < 4; i++) {
                mixer.ResetChannelParams(currentMixerSection * 4 + i);
            }
        }

        currentMenuIndex = newMenuIndex;
        redraw = true;
        hw.display.Fill(false);
    }

    if (hw.encoder.RisingEdge()) {
        if (currentMenu == MENU_SOUNDS) {
            currentKnobRow = (currentKnobRow + 1) % MENU_ROWS;
            redraw = true;
            drums[currentDrum]->ResetParams();
            screenOn = true;
        } else if (currentMenu == MENU_MIXER) {
            currentKnobRow = (currentKnobRow + 1) % MENU_ROWS;
            redraw = true;
            // reset params for mixer row?
            screenOn = true;
        }
    }

    if (screenOn) {
        usageCounter = 0;
        if (!screen.IsScreenOn()) {
            screen.SetScreenOn(true);
            hw.display.Fill(false);
        }
    }
    if (redraw) {
        DrawScreen(true);
        hw.display.Update();        
    }
}

// Process the current knob values and update model params accordingly.
// Knob number == param number, since model params are listed in UI order.
void ProcessKnobs() {

    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        float sig = hw.controls[knob].Value();
        if (currentMenu ==  0) {
            u8 param = currentKnobRow * KNOB_COUNT + knob;
            drums[currentDrum]->UpdateParam(param, sig);   // TODO bool changed = 
            if (std::abs(sig - lastKnobValue[knob]) > 0.1f) {    // TODO: use delta value from Param?
                usageCounter = 0;
                lastKnobValue[knob] = sig;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                } else {
                    DisplayKnobValues();
                }
            }
        } else {
            u8 channel = currentMixerSection * 4 + knob;
            bool changed = mixer.UpdateChannelParam(channel, currentKnobRow, sig);
            if (changed) {           // TODO combine for both menus at end of function
                usageCounter = 0;
                lastKnobValue[knob] = sig;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                } else {
                    DisplayKnobValues();
                }
            }
        }
    }
}

void ProcessControls()
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    ProcessEncoder();
    ProcessKnobs();
    // ProcessGates();
}


void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    meter.OnBlockStart();

    ProcessControls();

    for(size_t i = 0; i < size; i++) {

        cycle = (cycle + 1) % clockRange;
        if (cycle < clockThreshold) {
            // Process shared sound sources
            for (u8 i = 0; i < sourceCount; i++) {
                sources[i]->Process();
            }
            // if (ch.IsActive() || oh.IsActive() || cy.IsActive()) {
            //     // is always active, so we skip it when the sounds that use it aren't active
            //     source68.Process();
            // }

            if (CPU_SINGLE) {
                mixer.UpdateSignal(currentDrum, drums[currentDrum]->Process());
            } else {
                mixer.ResetSignals();
                for (uint8_t i = 0; i < drumCount; i++) {
                    mixer.UpdateSignal(i, drums[i]->Process());
                }
            }
        }

        mixer.Process();
        out[0][i] = mainGain * mixer.LeftSignal();
        out[1][i] = mainGain * mixer.RightSignal();
        out[2][i] = mixer.Send1Signal();
        out[3][i] = mixer.Send2Signal();
    }

    meter.OnBlockEnd();
}


// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{

    switch(m.type)
    {
        case NoteOn:
        {
            // NoteOnEvent p = m.AsNoteOn();
            // This is to avoid Max/MSP Note outs for now..
            // if(m.data[1] != 0)
            // {
            //     p = m.AsNoteOn();
            //     osc.SetFreq(mtof(p.note));
            //     osc.SetAmp((p.velocity / 127.0f));
            // } else {
            //     osc.SetAmp(0.0f);
            // }
            // screen.OledMessage("Midi: " + std::to_string(p.note) + ", " + std::to_string(p.velocity) + "     ", 3);

            NoteOnEvent p = m.AsNoteOn();
            float velocity = p.velocity / 127.0f;
            if (p.velocity > 0) {
                if (p.note >= MINIMUM_NOTE && p.note < MINIMUM_NOTE + drumCount) {
                    u8 drum = p.note - MINIMUM_NOTE;
                    drums[drum]->Trigger(velocity);
                    screen.ScreensaveEvent(drum);
                }
            }
        }
        break;
        case NoteOff:
        {
        }
        break;
        case ControlChange:
        {
            ControlChangeEvent p = m.AsControlChange();
            switch(p.control_number)
            {
                case 1:
                    break;
                case 2:
                    break;
                default: break;
            }
            break;
        }
        default: break;
    }
}


// Main -- Init, and Midi Handling
int main(void)
{
    // Init the hardware
    float samplerate;
    hw.Init(true);     // "true" boosts processor speed from 400mhz to 480mhz
    hw.SetAudioSampleRate(audioSampleRate);  // 8,16,32,48; higher rate requires more CPU
    samplerate = hw.AudioSampleRate();
    meter.Init(samplerate, 128, 1.0f);

    // Set up the kit and mixer
    InitKit(samplerate);
    mixer.Reset();
    currentDrum = 0;
    currentMenuIndex = 0;

    // send the toms out the send1
    mixer.SetChannelParam(2, Channel::PARAM_SEND1, 0.5);
    mixer.SetChannelParam(5, Channel::PARAM_SEND1, 0.5);
    mixer.SetChannelParam(7, Channel::PARAM_SEND1, 0.5);
    mixer.SetChannelParam(9, Channel::PARAM_SEND1, 0.5);

    // send percussion out send2
    mixer.SetChannelParam(11, Channel::PARAM_SEND2, 1);
    mixer.SetChannelParam(12, Channel::PARAM_SEND2, 1);
    mixer.SetChannelParam(14, Channel::PARAM_SEND2, 1);
    mixer.SetChannelParam(15, Channel::PARAM_SEND2, 1);

    // fill the menu
    for (u8 drum = 0; drum < drumCount; drum++) {
        screen.menuItems[drum] = drums[drum]->Slot();
    }
    for (u8 i = drumCount; i < Screen::MENU_SIZE; i++) {
        screen.menuItems[i] = "";
    }
    if (Screen::MENU_SIZE >= drumCount + 4) {
        screen.menuItems[drumCount] = "A";
        screen.menuItems[drumCount+1] = "B";
        screen.menuItems[drumCount+2] = "C";
        screen.menuItems[drumCount+3] = "D";
    }

    // initialize the knob tracking
    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    DrawScreen(true);

    // Start stuff.
    hw.SetAudioBlockSize(128);
    hw.midi.StartReceive();
    hw.StartAdc();
    hw.StartAudio(AudioCallback);
    for(;;)
    {
        hw.midi.Listen();
        while(hw.midi.HasEvents())
        {
            HandleMidiMessage(hw.midi.PopEvent());
        }

        DisplayKnobValues();

        float avgCpu = meter.GetAvgCpuLoad();
        screen.OledMessage("cpu:" + std::to_string((int)(avgCpu * 100)) + "%", 4, 10);
        screen.ShowCpu(avgCpu);

        usageCounter++;
        if (usageCounter > 3000) {    // 10000=about 90 seconds
            if (screen.IsScreenOn()) {
                screen.SetScreenOn(false);
            }
            screen.Screensave();
        }
        hw.display.Update();
    }
}
