#include "Runner.h"

using namespace daisy;
using namespace daisysp;

Runner* Runner::globalRunner = nullptr;

// Display the available parameter names.
void Runner::DisplayParamMenu() {

    screen.DrawRect(0, 9, 127, 36, false, true);

    // display labels for the parameters edited by the knobs
    u8 knobRow = currentKnobRow;
    std::string sc = "";
    std::string rc = "";
    ParamPage *paramPage = synth->GetParamPage(currentSynthPage);
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        sc = "";
        rc = "";
        if (currentMenu == MENU_SYNTH && currentSynthPage < synth->PageCount()) {
            if (knobRow > 0) {
                knobRow = 0;    // assumes synth pages only ever have one row
            }
            u8 param = knobRow * KNOB_COUNT + knob;
            sc = paramPage->GetParamName(param);
        } else if (currentMenu == MENU_MIXER) {
            if (knobRow > 1) {  // mixer only has 2 rows of params
                knobRow = 0;
            }
            u8 channel = currentMixerSection * 4 + knob;
            if (channel == 0) {
                sc = "Synth";
            }
            rc = mixer.GetChannelParamName(currentMixerSection * 4, knobRow);
        } else if (currentMenu == MENU_MIXER_MAIN) {
            // sc = mixer.GetGlobals()->GetParamName(knob);
        } else if (currentMenu == MENU_PATCH) {
            // sc = patchStorage.GetParamSet()->GetParamName(knob);
        }
        Rectangle rect2(knob * 32, 12, 32, 12);
        screen.DrawButton(rect2, sc, false, false, true);
    }
    Rectangle rect3(0, 24, 32, 12);
    screen.DrawButton(rect3, rc, false, false, true);
    screen.DrawPageTitle(paramPage->getModuleName(), paramPage->getTitle());

}

// Display the current values and parameter names of model params for 4 knobs.
// Knob number == param number, since model params are listed in UI order.
void Runner::DisplayKnobValues() {

    screen.DrawRect(0, 0, 127, 11, false, true);

    u8 knobRow = currentKnobRow;
    u8 index;
    ParamPage *paramPage = synth->GetParamPage(currentSynthPage);
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        std::string sc = "";
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SYNTH && currentSynthPage < synth->PageCount()) {
            if (knobRow > 0) {
                knobRow = 0;    // assumes synth pages only ever have one row
            }
            index = knobRow * KNOB_COUNT + knob;
            sc = paramPage->GetParamDisplay(index);
        } else if (currentMenu == MENU_MIXER) {
            if (knobRow > 1) {  // mixer only has 2 rows of params
                knobRow = 0;
            }
            index = currentMixerSection * 4 + knob;  // only using 1 channel & 1 mixer section
            sc = mixer.GetChannelParamDisplay(index, knobRow);
        } else if (currentMenu == MENU_MIXER_MAIN) {
            // sc = mixer.GetGlobals()->GetParamDisplay(knob);
        } else if (currentMenu == MENU_PATCH) {
            // switch (knob) {
            //     case 0:
            //         sc = "_" + std::to_string(currentPatch) + "_";
            //         break;
            //     case 1:
            //         sc = patchStorage.GetOperation();
            //         break;
            //     case 2:
            //         sc = patchStorage.GetParamSet()->GetParamDisplay(knob);
            //         break;
            // }
        }
        screen.WriteStringAligned(sc.c_str(), Font_6x8, rect, Alignment::centered, true);
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

    // Encoder turn
    int inc = hw.encoder.Increment();
    if (inc != 0) {
        screenOn = true;
    }
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, menuSize-1);
    if (newMenuIndex != currentMenuIndex && screen.IsScreenOn()) {
        if (newMenuIndex < synth->PageCount()) {
            currentMenu = MENU_SYNTH;
            currentSynthPage = newMenuIndex;
            synth->GetParamPage(currentSynthPage)->ResetParams();
        } else if (newMenuIndex < synth->PageCount() + mixerSections) {
            currentMenu = MENU_MIXER;
            currentMixerSection = newMenuIndex - synth->PageCount();
            for (u8 i = 0; i < 4; i++) {
                mixer.ResetChannelParams(currentMixerSection * 4 + i);
            }
        // } else if (newMenuIndex < kit->drumCount + mixerSections + 1) {
        //     currentMenu = MENU_MIXER_MAIN;
        //     mixer.GetGlobals()->ResetParams();
        // } else if (newMenuIndex < kit->drumCount + mixerSections + 2) {
        //     currentMenu = MENU_PATCH;
        //     patchStorage.GetParamSet()->ResetParams();
        }

        currentMenuIndex = newMenuIndex;
        redraw = true;
    }

    // Encoder press
    if (hw.encoder.RisingEdge() || hw.encoder.FallingEdge()) {
        screenOn = true;
    }
    if (hw.encoder.RisingEdge() && screen.IsScreenOn()) {
        lastEncoderTime = System::GetNow();
    }
    if (hw.encoder.FallingEdge() && screen.IsScreenOn()) {
        if (System::GetNow() - lastEncoderTime > LONG_PRESS_MILLIS) {
            // saveTo = currentPatch;
        } else {
            if (currentMenu == MENU_SYNTH) {
                // do nothing for now
                screenOn = true;
            } else if (currentMenu == MENU_MIXER) {
                currentKnobRow = (currentKnobRow + 1) % MENU_ROWS;
                redraw = true;
                // reset params for mixer row?
                screenOn = true;
            } else if (currentMenu == MENU_MIXER_MAIN) {
                // nothing happens
            } else if (currentMenu == MENU_PATCH) {
                // u8 target = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_TARGET_PATCH);
                // u8 operation = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_OPERATION);
                // switch (operation) {
                //     case PatchStorage::OPERATION_LOAD:
                //         currentPatch = target;
                //         patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                //         loadFrom = target;
                //         break;
                //     case PatchStorage::OPERATION_SAVE:
                //         currentPatch = target;
                //         patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                //         saveTo = target;
                //         break;
                // }
            }
        }
    }

    if (screenOn) {
        lastScreenTime = System::GetNow();
        if (!screen.IsScreenOn()) {
            screen.SetScreenOn(true);
            redraw = true;
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

    bool anyChanged = false;
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        bool changed = false;
        float sig = hw.controls[knob].Value();
        if (currentMenu ==  MENU_SYNTH) {
            u8 param = currentKnobRow * KNOB_COUNT + knob;
            changed = synth->GetParamPage(currentSynthPage)->UpdateParam(param, sig);
        } else if (currentMenu == MENU_MIXER) {
            u8 channel = currentMixerSection * 4 + knob;
            changed = mixer.UpdateChannelParam(channel, currentKnobRow, sig);
        // } else if (currentMenu == MENU_MIXER_MAIN) {
        //     bool changed = mixer.GetGlobals()->UpdateParam(knob, sig);
        //     if (changed) {
        //         lastScreenTime = System::GetNow();
        //         lastKnobValue[knob] = sig;
        //         if (!screen.IsScreenOn()) {
        //             screen.SetScreenOn(true);
        //             DrawScreen(true);
        //         }
        //         DisplayKnobValues();
        //     }
        // } else if (currentMenu == MENU_PATCH) {
        //     bool changed = patchStorage.GetParamSet()->UpdateParam(knob, sig);
        //     if (changed) {
        //         lastScreenTime = System::GetNow();
        //         lastKnobValue[knob] = sig;
        //         if (!screen.IsScreenOn()) {
        //             screen.SetScreenOn(true);
        //             DrawScreen(true);
        //         }
        //         DisplayKnobValues();
        //     }
        }
        if (changed) {
            lastKnobValue[knob] = sig;
            anyChanged = true;
        }
    }

    if (anyChanged) {
        lastScreenTime = System::GetNow();
        if (!screen.IsScreenOn()) {
            screen.SetScreenOn(true);
            DrawScreen(true);
        }
        DisplayKnobValues();
    }
}

void Runner::ProcessControls() {
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    ProcessEncoder();
    ProcessKnobs();
    // ProcessGates();
}


void Runner::AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    meter.OnBlockStart();
    ProcessControls();
    for(size_t i = 0; i < size; i++) {
        // cycle = (cycle + 1) % clockRange;
        // // if (cycle < clockThreshold) {
        //     float signal = synth->Process();
        //     mixer.ResetSignals();
        //     mixer.UpdateSignal(0, signal);
        // // }
        synth->Process();
        out[0][i] = synth->GetOutput(0);
        out[1][i] = synth->GetOutput(1);
        out[2][i] = 0;
        out[3][i] = 0;
        // mixer.Process();
        // out[0][i] = mainGain * mixer.LeftSignal();
        // out[1][i] = mainGain * mixer.RightSignal();
        // out[2][i] = mixer.Send1Signal();
        // out[3][i] = mixer.Send2Signal();

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
    data3[1] = m.data[0];
    data3[2] = m.data[1];
    switch (m.type) {
        case NoteOn: {
            data3[0] = (m.channel & 0x0F) + 0x90;
            hw.midi.SendMessage(data3, 3);
            break;
        }
        case NoteOff: {
            data3[0] = (m.channel & 0x0F) + 0x80;
            hw.midi.SendMessage(data3, 3);
            break;
        }
        case ControlChange: {
            data3[0] = (m.channel & 0x0F) + 0xb0;
            hw.midi.SendMessage(data3, 3);
            break;
        }
        case ProgramChange: {
            data3[0] = (m.channel & 0x0F) + 0xc0;
            hw.midi.SendMessage(data3, 2);
            break;
        }
    }

}

// Typical Switch case for Message Type.
void Runner::HandleMidiMessage(MidiEvent m) {

    // will pass it through if it can
    // MidiSend(m);

    switch(m.type) {
        case NoteOn: {
            NoteOnEvent event = m.AsNoteOn();
            // screen.OledMessage("Note:" + std::to_string(event.channel) + ":" + std::to_string(event.note), 3);
            if (event.channel == midiChannel) {
                float velocity = (float)event.velocity / 127.0f;
                if (velocity > 0) {
                    synth->NoteOn(event.note, velocity);
                    screen.ScreensaveEvent(event.note);
                } else {
                    synth->NoteOff(event.note);
                }
            }
            break;
        }
        case NoteOff: {
            NoteOffEvent event = m.AsNoteOff();
            synth->NoteOff(event.note);
            break;
        }
        case ControlChange: {
            // ControlChangeEvent event = m.AsControlChange();
            // if (event.channel == midiChannel) {
            //     float sig = (float)event.value / 127;
            //     // send to synth
            // }
            break;
        }
        case ProgramChange: {
            break;
        }
        default: 
            break;
    }
}

void Runner::Run(ISynth *synth) {
    if (samplerate == 0) {
        // print error
        return;
    }

    this->synth = synth;
    mixer.Reset();

    // initialize the knob tracking
    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    DrawScreen(true);

    // MidiUsbHandler::Config midi_cfg;
    // midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    // usbMidi.Init(midi_cfg);

    // Start stuff.
    hw.SetAudioBlockSize(64);
    hw.midi.StartReceive();
    // usbMidi.StartReceive();
    hw.StartAdc();
    Runner::globalRunner = this;
    hw.StartAudio(GlobalAudioCallback);  

    u32 clock = 0;
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

        if (clock == 0) {
            DisplayKnobValues();
            hw.display.Update();
        }

        if (SHOW_CPU) {
            float avgCpu = meter.GetAvgCpuLoad();
            screen.OledMessage("cpu:" + std::to_string((int)(avgCpu * 100)) + "%", 4, 10);
            screen.ShowCpu(avgCpu);
        }

        u32 now = System::GetNow();
        if (now - lastScreenTime > SCREEN_SAVE_MILLIS) {
            if (screen.IsScreenOn()) {
                screen.SetScreenOn(false);
            }
            screen.Screensave(now);
        }

        clock = (clock + 1) % UPDATE_CLOCK_TICKS;
    }
}

void Runner::GlobalAudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    if (globalRunner != nullptr) {
        globalRunner->AudioCallback(in, out, size);
    }
}