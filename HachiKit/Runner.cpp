#include "Runner.h"

using namespace daisy;
using namespace daisysp;

Runner* Runner::globalRunner = nullptr;

// Display the available parameter names.
void Runner::DisplayParamMenu() {

    screen.DrawRect(0, 9, 127, 36, false, true);

    // TODO make this work for mixer channels
    u8 knobRow = currentKnobRow;
    // if (knobRow * KNOB_COUNT >= drums[currentDrum]->ParamCount()) {
    //     knobRow = 0;
    // }

    std::string sc = "";
    bool selected = false;
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        for (u8 row = 0; row < MENU_ROWS; row++) {
            Rectangle rect2(knob * 32, (row + 1) * 12, 32, 12);
            if (currentMenu == MENU_SOUNDS && kit->drums[currentDrum] != nullptr) {
                u8 param = row * KNOB_COUNT + knob;
                sc = kit->drums[currentDrum]->GetParamName(param);
                selected = row == knobRow;
            } else if (currentMenu == MENU_MIXER) {
                u8 channel = currentMixerSection * 4 + knob;
                if (channel < kit->drumCount && kit->drums[channel] != nullptr) {
                    if (row == 0) {
                        selected = false;
                        sc = kit->drums[channel]->Slot();  // show channel names on first row
                    } else if (row == 1) {
                        sc = mixer.GetChannelParamName(channel, knob);
                        selected = knobRow == knob;   // for mixer, we show the selections on the 2nd row
                    }
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
void Runner::DisplayKnobValues() {

    screen.DrawRect(0, 0, 127, 11, false, true);

    u8 knobRow = currentKnobRow;
    // make this work for mixer rows
    // if (knobRow * KNOB_COUNT >= kit->drums[currentDrum]->ParamCount()) {
    //     knobRow = 0;
    // }

    u8 index;
    std::string sc = "";
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SOUNDS && kit->drums[currentDrum] != nullptr) {
            index = knobRow * KNOB_COUNT + knob;
            sc = kit->drums[currentDrum]->GetParamString(index);
        } else if (currentMenu == MENU_MIXER) {
            index = currentMixerSection * 4 + knob;
            if (index < kit->drumCount && kit->drums[index] != nullptr) {
                sc = mixer.GetChannelParamDisplay(index, knobRow);
            }
        }
        screen.WriteStringAligned(sc.c_str(), Font_6x8, rect, Alignment::centered, true);
        // screen.DrawButton(rect, sc, false, true, false);
    }
}

void Runner::DrawScreen(bool clearFirst) {
    if (clearFirst) {
        hw.display.Fill(false);
    }
    screen.DrawMenu(currentMenuIndex);
    DisplayParamMenu();
    DisplayKnobValues();
    hw.display.Update();        
}

void Runner::ProcessEncoder() {

    bool redraw = false;
    bool screenOn = false;

    int inc = hw.encoder.Increment();
    if (inc != 0) {
        screenOn = true;
    }
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, Screen::MENU_SIZE-1);
    // u8 newMenuIndex = (currentMenuIndex + inc) % Screen::MENU_SIZE;
    if (newMenuIndex != currentMenuIndex) {
        if (newMenuIndex < kit->drumCount) {
            currentMenu = MENU_SOUNDS;
            u8 newDrum = newMenuIndex;
            if (kit->drums[newDrum] != nullptr) {
                kit->drums[newDrum]->ResetParams();
            }
            currentDrum = newDrum;
        } else {
            currentMenu = MENU_MIXER;
            currentMixerSection = newMenuIndex - kit->drumCount;
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
            if (kit->drums[currentDrum] != nullptr) {
                kit->drums[currentDrum]->ResetParams();
            }
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
void Runner::ProcessKnobs() {

    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        float sig = hw.controls[knob].Value();
        if (currentMenu ==  MENU_SOUNDS) {
            u8 param = currentKnobRow * KNOB_COUNT + knob;
            if (kit->drums[currentDrum] != nullptr) {
                kit->drums[currentDrum]->UpdateParam(param, sig);   // TODO bool changed = 
            }
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
        } else if (currentMenu == MENU_MIXER) {
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

void Runner::ProcessControls()
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    ProcessEncoder();
    ProcessKnobs();
    // ProcessGates();
}


void Runner::AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    meter.OnBlockStart();

    ProcessControls();

    for(size_t i = 0; i < size; i++) {

        cycle = (cycle + 1) % clockRange;
        if (cycle < clockThreshold) {
            // Process shared sound sources
            for (u8 i = 0; i < kit->sourceCount; i++) {
                kit->sources[i]->Process();
            }
            // if (ch.IsActive() || oh.IsActive() || cy.IsActive()) {
            //     // is always active, so we skip it when the sounds that use it aren't active
            //     source68.Process();
            // }

            if (CPU_SINGLE && kit->drums[currentDrum] != nullptr) {
                mixer.UpdateSignal(currentDrum, kit->drums[currentDrum]->Process());
            } else {
                mixer.ResetSignals();
                for (uint8_t i = 0; i < kit->drumCount; i++) {
                    if (kit->drums[i] != nullptr) {
                        mixer.UpdateSignal(i, kit->drums[i]->Process());
                    }
                }
            }
        }

        mixer.Process();
        out[0][i] = mainGain * mixer.LeftSignal();
        out[1][i] = mainGain * mixer.RightSignal();
        out[2][i] = mixer.Send1Signal();
        out[3][i] = mixer.Send2Signal();

        // combine the audio inputs with the corresponding outputs
        if (AUDIO_PASSTHRU) {
            out[0][i] += in[0][i];
            out[1][i] += in[1][i];
            out[2][i] += in[2][i];
            out[3][i] += in[3][i];
        }

    }

    meter.OnBlockEnd();
}


void Runner::MidiSend(MidiEvent m) {

    u8 data3[3];
    u8 length = 0;
    switch (m.type) {
        case NoteOn: {
            length = 3;
            data3[0] = (m.channel & 0x0F) + 0x90;
            break;
        }
        case NoteOff: {
            data3[0] = (m.channel & 0x0F) + 0x80;
            break;
        }
        case ControlChange: {
            data3[0] = (m.channel & 0x0F) + 0xb0;
            break;
        }
    }

    if (length == 3) {
        data3[1] = m.data[0];
        data3[2] = m.data[1];
        hw.midi.SendMessage(data3, 3);
    }

}


// Typical Switch case for Message Type.
void Runner::HandleMidiMessage(MidiEvent m)
{

    // will pass it through if it can
    MidiSend(m);

    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
            if (p.channel == midiChannel) {
                float velocity = p.velocity / 127.0f;
                if (p.velocity > 0) {
                    if (p.note >= MINIMUM_NOTE && p.note < MINIMUM_NOTE + MIDIMAP_SIZE) {
                        u8 n = p.note - MINIMUM_NOTE;
                        if (kit->midiMap[n] != nullptr) {
                            kit->midiMap[n]->Trigger(velocity);
                        }
                        screen.ScreensaveEvent(n);
                    }
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

void Runner::Run(Kit *kit) {
    if (samplerate == 0) {
        // print error
        return;
    }

    this->kit = kit;

    // Set up the kit and mixer
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
    for (u8 drum = 0; drum < kit->drumCount; drum++) {
        if (kit->drums[drum] != nullptr) {
            screen.menuItems[drum] = kit->drums[drum]->Slot();
        }
    }
    for (u8 i = kit->drumCount; i < Screen::MENU_SIZE; i++) {
        screen.menuItems[i] = "";
    }
    if (Screen::MENU_SIZE >= kit->drumCount + 4) {
        screen.menuItems[kit->drumCount] = "A";
        screen.menuItems[kit->drumCount+1] = "B";
        screen.menuItems[kit->drumCount+2] = "C";
        screen.menuItems[kit->drumCount+3] = "D";
    }

    // initialize the knob tracking
    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    DrawScreen(true);

    // MidiUsbHandler::Config midi_cfg;
    // midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    // usbMidi.Init(midi_cfg);

    // Start stuff.
    hw.SetAudioBlockSize(128);
    hw.midi.StartReceive();
    // usbMidi.StartReceive();
    hw.StartAdc();
    Runner::globalRunner = this;
    hw.StartAudio(GlobalAudioCallback);
    for(;;)
    {
        hw.midi.Listen();
        while(hw.midi.HasEvents())
        {
            HandleMidiMessage(hw.midi.PopEvent());
        }

        // usbMidi.Listen();
        // while(usbMidi.HasEvents())
        // {
        //     HandleMidiMessage(usbMidi.PopEvent());
        // }

        DisplayKnobValues();

        float avgCpu = meter.GetAvgCpuLoad();
        screen.OledMessage("cpu:" + std::to_string((int)(avgCpu * 100)) + "%", 4, 10);
        screen.ShowCpu(avgCpu);

        usageCounter++;
        if (usageCounter > 10000) {    // 10000=about 90 seconds
            if (screen.IsScreenOn()) {
                screen.SetScreenOn(false);
            }
            screen.Screensave();
        }
        hw.display.Update();
    }
}

void Runner::GlobalAudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    if (globalRunner != nullptr) {
        globalRunner->AudioCallback(in, out, size);
    }
}