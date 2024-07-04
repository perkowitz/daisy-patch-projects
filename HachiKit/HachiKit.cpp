#include <chrono>
#include <thread>

#include "daisy_patch.h"
#include "daisysp.h"
#include "util/CpuLoadMeter.h"
#include <string>
#include "Utility.h"
#include "audio/Mixer16.h"
#include "audio/Processing.h"
#include "Screen.h"
#include "IDrum.h"
#include "sounds/Bd8.h"
#include "sounds/Ch.h"
#include "sounds/Clap.h"
#include "sounds/Clave8.h"
#include "sounds/ClickSource.h"
#include "sounds/Cow8.h"
#include "sounds/Cy.h"
#include "sounds/DigiClap.h"
#include "sounds/FmDrum.h"
#include "sounds/HhSource68.h"
#include "sounds/MultiTomSource.h"
#include "sounds/MultiTom.h"
#include "sounds/Oh.h"
#include "sounds/Sd8.h"
#include "sounds/SdNoise.h"
#include "sounds/Tom.h"

using namespace daisy;
using namespace daisysp;

#define MINIMUM_NOTE 36
#define KNOB_COUNT 4
#define CPU_SINGLE false

DaisyPatch hw;
Screen screen(&hw.display);
CpuLoadMeter meter;

Mixer16 mixer;

IDrum *drums[16];
uint8_t drumCount = 1;
Bd8 bd;
SdNoise rs;
Sd8 sd;
Clap cp;
DigiClap sd2;
Tom lt, mt, ht;
// MultiTom lt, mt, ht;
Ch ch;
Oh oh;
SdNoise ma;
Cy cy;
Cow8 cb;
FmDrum fm1, fm2;
Clave8 cl;

// Shared sound sources
HhSource68 source68;
ClickSource clickSource;
// MultiTomSource multiTomSource;

u8 currentMenu = 0; 
u8 currentMenuIndex = 0;
uint8_t currentDrum = 0;
uint8_t currentKnobRow = 0;
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
    if (currentMenu != 0) return;

    screen.DrawRect(0, 9, 127, 36, false, true);

    u8 knobRow = currentKnobRow;
    if (knobRow * KNOB_COUNT >= drums[currentDrum]->ParamCount()) {
        knobRow = 0;
    }

    uint8_t param;
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        // for (u8 row = 0; row <= drums[currentDrum]->PARAM_COUNT / 4; row++) {
        for (u8 row = 0; row < 2; row++) {
            Rectangle rect2(knob * 32, (row + 1) * 12, 32, 12);
            param = row * KNOB_COUNT + knob;
            std::string sc = drums[currentDrum]->GetParamName(param);
            bool selected = row == knobRow;
            // hw.display.WriteStringAligned(sc.c_str(), Font_6x8, rect2, Alignment::centered, true);
            screen.DrawButton(rect2, sc, selected, selected, !selected);
            // hw.display.SetCursor(rect2.GetX(), rect2.GetY());
            // hw.display.WriteString(sc.c_str(), Font_6x8, true);
            // hw.display.DrawLine(0, rect2.GetY() + 11, 127, rect2.GetY() + 11, true);
        }
    }
}

// Display the current values and parameter names of model params for 4 knobs.
// Knob number == param number, since model params are listed in UI order.
void DisplayKnobValues() {
    if (currentMenu != 0) return;

    screen.DrawRect(0, 0, 127, 11, false, true);

    uint8_t param;
    u8 knobRow = currentKnobRow;
    if (knobRow * KNOB_COUNT >= drums[currentDrum]->ParamCount()) {
        knobRow = 0;
    }
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        param = knobRow * KNOB_COUNT + knob;
        Rectangle rect(knob * 32, 0, 32, 8);
        std::string sc = drums[currentDrum]->GetParamString(param);
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

    int inc = hw.encoder.Increment();
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, Screen::MENU_SIZE-1);
    if (newMenuIndex != currentMenuIndex) {
        if (newMenuIndex < drumCount) {
            currentMenu = 0;
            u8 newDrum = newMenuIndex;
            drums[newDrum]->ResetParams();
            currentDrum = newDrum;
        } else {
            currentMenu = 1;
        }

        currentMenuIndex = newMenuIndex;
        usageCounter = 0;
        screen.SetScreenOn(true);
        redraw = true;
        hw.display.Fill(false);
    }

    if (hw.encoder.RisingEdge()) {
        if (currentMenu == 0) {
            currentKnobRow = (currentKnobRow + 1) % 2;
            redraw = true;
            drums[currentDrum]->ResetParams();
            usageCounter = 0;
            screen.SetScreenOn(true);
            hw.display.Fill(false);
        }
    }

    if (redraw) {
        DrawScreen(false);
        hw.display.Update();        
    }
}

// Process the current knob values and update model params accordingly.
// Knob number == param number, since model params are listed in UI order.
void ProcessKnobs() {

    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        float sig = hw.controls[knob].Value();
        if (currentMenu ==  0) {
            uint8_t param = currentKnobRow * KNOB_COUNT + knob;
            drums[currentDrum]->UpdateParam(param, sig);
            if (std::abs(sig - lastKnobValue[knob]) > 0.1f) {    // TODO: use delta value from Param?
                usageCounter = 0;
                screen.SetScreenOn(true);
                lastKnobValue[knob] = sig;
                DrawScreen(true);
            }
        } else {
            switch (knob) {
                case 0:
                    mainGain = sig < 0.5 ? sig * 2 : sig * 4 - 1;
                    if (mainGain < 0.1) {
                        mainGain = 0;
                    }
                    break;
                case 1:
                    if (sig <= 0.1) {
                        clockThreshold = clockRange / 4;
                    } else if (sig <= 0.3) {
                        clockThreshold = clockRange / 2;
                    } else {
                        clockThreshold = (int)(((sig - 0.3) / 7 * 6 + 0.5) * clockRange);
                    }
                    break;
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
            source68.Process();
            clickSource.Process();
            // multiTomSource.Process();

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
    // Init
    float samplerate;
    hw.Init(true);     // "true" boosts processor speed from 400mhz to 480mhz
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_32KHZ);  // 8,16,32,48; higher rate requires more CPU
    samplerate = hw.AudioSampleRate();

    meter.Init(samplerate, 128, 1.0f);

    mixer.Reset();

    // Shared sound sources
    source68.Init("", samplerate, HhSource68::MORPH_808_VALUE);
    clickSource.Init("", samplerate, 1500, 191, 116);
    // multiTomSource.Init("", samplerate, 500, &clickSource);

    bd.Init("BD", samplerate, 78, 0.001, 4, 0.001, 0.5, 20);
    rs.Init("RS", samplerate);
    sd.Init("SD", samplerate);
    cp.Init("CP", samplerate, 0.012, 0.8);
    sd2.Init("S2", samplerate, 0.012, 0.8, 3000);

    lt.Init("LT", samplerate, 80, &clickSource);
    mt.Init("MT", samplerate, 91, &clickSource);
    ht.Init("HT", samplerate, 106, &clickSource);
    // lt.Init("LT", samplerate, 80, 0.8, &multiTomSource, 0);
    // mt.Init("MT", samplerate, 91, 0.8, &multiTomSource, 1);
    // ht.Init("HT", samplerate, 106, 0.8, &multiTomSource, 2);


    ch.Init("CH", samplerate, 0.001, 0.5, &source68, HhSource68::MORPH_808_VALUE, 6000, 16000);
    oh.Init("OH", samplerate, 0.001, 0.13, 0.001, &source68, HhSource68::MORPH_808_VALUE, 6000, 16000);
    ma.Init("MA", samplerate);
    cy.Init("CY", samplerate, 0.001, 3.5, &source68, 1700, 2400);
    cb.Init("CB", samplerate, 0.005, 0.5, &source68, 1700, 2400);
    fm1.Init("LC", samplerate, 98, 3.3, 2.2, 0.001, 0.101, -50);
    fm2.Init("HC", samplerate, 131, 3.3, 2.2, 0.001, 0.101, -50);
    cl.Init("CL", samplerate, 2000, 0.375);

    drums[0] = &bd;
    drums[1] = &rs;
    drums[2] = &sd;
    drums[3] = &cp;
    drums[4] = &sd2;
    drums[5] = &lt;
    drums[6] = &ch;
    drums[7] = &mt;
    drums[8] = &ma;
    drums[9] = &ht;
    drums[10] = &oh;
    drums[11] = &fm1;
    drums[12] = &fm2;
    drums[13] = &cy;
    drums[14] = &cl;
    drums[15] = &cb;
    drumCount = 16;
    currentDrum = 0;
    currentMenuIndex = 0;

    // send the toms out the send1
    mixer.SetSend1(5, 1);
    mixer.SetSend1(7, 1);
    mixer.SetSend1(9, 1);

    // send percussion out send2
    mixer.SetSend2(11, 1);
    mixer.SetSend2(12, 1);
    mixer.SetSend2(14, 1);
    mixer.SetSend2(15, 1);

    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    //display
    hw.display.Fill(false);
    // screen.OledMessage("Hachikit 0.1", 5);
    // Utility::DrawDrums(&hw.display, 5);
    screen.DrawMenu(currentMenuIndex);
    DisplayParamMenu();
    hw.display.Update();

    // fill the menu
    for (u8 drum = 0; drum < drumCount; drum++) {
        screen.menuItems[drum] = drums[drum]->Slot();
    }
    for (u8 i = drumCount; i < Screen::MENU_SIZE; i++) {
        screen.menuItems[i] = "";
    }
    if (Screen::MENU_SIZE >= drumCount + 1) {
        screen.menuItems[drumCount] = "Fx";
    }
    if (Screen::MENU_SIZE >= drumCount + 5) {
        screen.menuItems[drumCount+1] = "A";
        screen.menuItems[drumCount+2] = "B";
        screen.menuItems[drumCount+3] = "C";
        screen.menuItems[drumCount+4] = "D";
    }

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

        usageCounter++;
        if (usageCounter > 1000) {    // 10000=about 90 seconds
            if (screen.IsScreenOn()) {
                screen.SetScreenOn(false);
            }
            screen.Screensave();
        }
        hw.display.Update();
    }
}
