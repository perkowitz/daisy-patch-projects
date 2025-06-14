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
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        sc = "";
        rc = "";
        if (currentMenu == MENU_SOUNDS && kit->drums[currentDrum] != nullptr) {
            if (knobRow * KNOB_COUNT >= kit->drums[currentDrum]->ParamCount()) {
                knobRow = 0;
            }
            // needs to be updated if MENU_ROWS becomes > 2          
            if (kit->drums[currentDrum]->ParamCount() >= 4 && knobRow == 0) {
                rc = "o.";
            } else if (kit->drums[currentDrum]->ParamCount() >= 4 && knobRow == 1) {
                rc = ".o";
            }
            u8 param = knobRow * KNOB_COUNT + knob;
            sc = kit->drums[currentDrum]->GetParamName(param);
        } else if (currentMenu == MENU_MIXER) {
            if (knobRow > 1) {  // mixer only has 2 rows of params
                knobRow = 0;
            }
            u8 channel = currentMixerSection * 4 + knob;
            if (channel < kit->drumCount && kit->drums[channel] != nullptr) {
                sc = kit->drums[channel]->Slot();  // show channel names on first row
            }
            rc = mixer.GetChannelParamName(currentMixerSection * 4, knobRow);
        } else if (currentMenu == MENU_MIXER_MAIN) {
            sc = mixer.GetGlobals()->GetParamName(knob);
        } else if (currentMenu == MENU_PATCH) {
            sc = patchStorage.GetParamSet()->GetParamName(knob);
        }
        Rectangle rect2(knob * 32, 12, 32, 12);
        screen.DrawButton(rect2, sc, false, false, true);
    }
    Rectangle rect3(0, 24, 32, 12);
    screen.DrawButton(rect3, rc, false, false, true);

}

// Display the current values and parameter names of model params for 4 knobs.
// Knob number == param number, since model params are listed in UI order.
void Runner::DisplayKnobValues() {

    screen.DrawRect(0, 0, 127, 11, false, true);

    u8 knobRow = currentKnobRow;
    u8 index;
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        std::string sc = "";
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SOUNDS && kit->drums[currentDrum] != nullptr) {
            if (knobRow * KNOB_COUNT >= kit->drums[currentDrum]->ParamCount()) {
                knobRow = 0;
            }
            index = knobRow * KNOB_COUNT + knob;
            // if (knob == currentKnob) {
                sc = kit->drums[currentDrum]->GetParamString(index);
            // }
        } else if (currentMenu == MENU_MIXER) {
            if (knobRow > 1) {  // mixer only has 2 rows of params
                knobRow = 0;
            }
            index = currentMixerSection * 4 + knob;
            if (index < kit->drumCount && kit->drums[index] != nullptr) {
                sc = mixer.GetChannelParamDisplay(index, knobRow);
            }
        } else if (currentMenu == MENU_MIXER_MAIN) {
            sc = mixer.GetGlobals()->GetParamDisplay(knob);
        } else if (currentMenu == MENU_PATCH) {
            switch (knob) {
                case 0:
                    sc = "_" + std::to_string(currentPatch) + "_";
                    break;
                case 1:
                    sc = patchStorage.GetOperation();
                    break;
                case 2:
                    sc = patchStorage.GetParamSet()->GetParamDisplay(knob);
                    break;
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

// Make changes that need to be made as a result of encoder changes.
// This is called outside the audio loop, so can include updates that
// may be costly & can happen more slowly (e.g. updating the screen)
void Runner::UpdateFromEncoder() {

    if (newMenuIndex != currentMenuIndex && screen.IsScreenOn()) {
        if (newMenuIndex < kit->drumCount) {
            currentMenu = MENU_SOUNDS;
            u8 newDrum = newMenuIndex;
            if (kit->drums[newDrum] != nullptr) {
                kit->drums[newDrum]->ResetParams();
            }
            currentDrum = newDrum;
        } else if (newMenuIndex < kit->drumCount + mixerSections) {
            currentMenu = MENU_MIXER;
            currentMixerSection = newMenuIndex - kit->drumCount;
            for (u8 i = 0; i < 4; i++) {
                mixer.ResetChannelParams(currentMixerSection * 4 + i);
            }
        } else if (newMenuIndex < kit->drumCount + mixerSections + 1) {
            currentMenu = MENU_MIXER_MAIN;
            mixer.GetGlobals()->ResetParams();
        } else if (newMenuIndex < kit->drumCount + mixerSections + 2) {
            currentMenu = MENU_PATCH;
            patchStorage.GetParamSet()->ResetParams();
        }

        currentMenuIndex = newMenuIndex;
        redrawScreen = true;
        // hw.display.Fill(false);
    }

    if (turnScreenOn) {
        lastScreenTime = System::GetNow();
        if (!screen.IsScreenOn()) {
            screen.SetScreenOn(true);
            redrawScreen = true;
        }
    }
    if (redrawScreen) {
        DrawScreen(true);
        hw.display.Update();        
    }

    redrawScreen = false;
    turnScreenOn = false;
}

// Check the encoder for push/turn and respond accordingly.
void Runner::ProcessEncoder() {

    int inc = hw.encoder.Increment();
    if (inc != 0) {
        turnScreenOn = true;
    }
    newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, menuSize-1);

    if (hw.encoder.RisingEdge() || hw.encoder.FallingEdge()) {
        turnScreenOn = true;
    }
    if (hw.encoder.RisingEdge() && screen.IsScreenOn()) {
        lastEncoderTime = System::GetNow();
    }
    if (hw.encoder.FallingEdge() && screen.IsScreenOn()) {
        if (System::GetNow() - lastEncoderTime > LONG_PRESS_MILLIS) {
            saveTo = currentPatch;
        } else {
            if (currentMenu == MENU_SOUNDS) {
                currentKnobRow = (currentKnobRow + 1) % MENU_ROWS;
                redrawScreen = true;
                if (kit->drums[currentDrum] != nullptr) {
                    kit->drums[currentDrum]->ResetParams();
                }
                turnScreenOn = true;
            } else if (currentMenu == MENU_MIXER) {
                currentKnobRow = (currentKnobRow + 1) % MENU_ROWS;
                redrawScreen = true;
                // reset params for mixer row?
                turnScreenOn = true;
            } else if (currentMenu == MENU_MIXER_MAIN) {
                // nothing happens
            } else if (currentMenu == MENU_PATCH) {
                u8 target = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_TARGET_PATCH);
                u8 operation = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_OPERATION);
                switch (operation) {
                    case PatchStorage::OPERATION_PRESET:
                        patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                        loadFrom = target;
                        break;
                    case PatchStorage::OPERATION_LOAD:
                        currentPatch = target;
                        patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                        loadFrom = target;   // only using presets for now
                        break;
                    case PatchStorage::OPERATION_SAVE:
                        currentPatch = target;
                        patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                        saveTo = target;
                        break;
                }
            }
        }
    }
    UpdateFromEncoder();

}

// Process the current knob values and update model params accordingly.
// Knob number == param number, since model params are listed in UI order.
void Runner::ProcessKnobs() {

    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        float sig = hw.controls[knob].Value();
        if (currentMenu ==  MENU_SOUNDS) {
            u8 param = currentKnobRow * KNOB_COUNT + knob;
            if (kit->drums[currentDrum] != nullptr) {
                if (param >= kit->drums[currentDrum]->ParamCount()) {
                    param = param % KNOB_COUNT;
                }
                kit->drums[currentDrum]->UpdateParam(param, sig);   // TODO bool changed = 
            }
            if (std::abs(sig - lastKnobValue[knob]) > 0.1f) {    // TODO: use delta value from Param?
                lastScreenTime = System::GetNow();
                lastKnobValue[knob] = sig;
                currentKnob = knob;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                }
                DisplayKnobValues();
            }
        } else if (currentMenu == MENU_MIXER) {
            u8 channel = currentMixerSection * 4 + knob;
            bool changed = mixer.UpdateChannelParam(channel, currentKnobRow, sig);
            if (changed) {           // TODO combine for both menus at end of function
                lastScreenTime = System::GetNow();
                lastKnobValue[knob] = sig;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                }
                DisplayKnobValues();
            }
        } else if (currentMenu == MENU_MIXER_MAIN) {
            bool changed = mixer.GetGlobals()->UpdateParam(knob, sig);
            if (changed) {
                lastScreenTime = System::GetNow();
                lastKnobValue[knob] = sig;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                }
                DisplayKnobValues();
            }
        } else if (currentMenu == MENU_PATCH) {
            bool changed = patchStorage.GetParamSet()->UpdateParam(knob, sig);
            if (changed) {
                lastScreenTime = System::GetNow();
                lastKnobValue[knob] = sig;
                if (!screen.IsScreenOn()) {
                    screen.SetScreenOn(true);
                    DrawScreen(true);
                }
                DisplayKnobValues();
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

void Runner::CvOut(u8 output, float signal) {
    DacHandle::Channel ch = output == 0 ? DacHandle::Channel::ONE : DacHandle::Channel::TWO;
    if (output < CV_OUT_COUNT) {
        hw.seed.dac.WriteValue(ch, static_cast<uint16_t>(CV_OUT_SCALE_FACTOR * signal));
    }
}

void Runner::GateOut(bool value) {
    dsy_gpio_write(&hw.gate_output, value);
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

void Runner::Load(u8 patch, Runner::Kit *kit, PersistentStorage<KitPatch> *savedKit) {
    if (patch >= PATCH_COUNT) return;
    
	KitPatch &kitPatch = savedKit->GetSettings();
    for (u8 d = 0; d < std::min((u8)DRUMS_IN_PATCH, kit->drumCount); d++) {
        if (kit->drums[d] != nullptr) {
            IDrum *drum = kit->drums[d];
            for (u8 p = 0; p < std::min((u8)PATCH_SIZE, drum->ParamCount()); p++) {
                drum->SetParam(p, kitPatch.drumPatches[d].params[p]);
            }
        }        
    }

}

void Runner::SaveToKitPatch(Runner::Kit *kit, Runner::KitPatch *kitPatch) {
    for (u8 d = 0; d < std::min((u8)DRUMS_IN_PATCH, kit->drumCount); d++) {
        if (kit->drums[d] != nullptr) {
            IDrum *drum = kit->drums[d];
            for (u8 p = 0; p < std::min((u8)PATCH_SIZE, drum->ParamCount()); p++) {
                kitPatch->drumPatches[d].params[p] = drum->GetParam(p);
            }
        }        
    }  
}

void Runner::Save(u8 patch, Runner::Kit *kit, PersistentStorage<KitPatch> *savedKit) {
    if (patch >= PATCH_COUNT) return;

    Runner::KitPatch &kitPatch = savedKit->GetSettings();
    SaveToKitPatch(kit, &kitPatch);
    savedKit->Save();
    System::Delay(100);    
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
        case SystemRealTime: {
            u8 data1[1];
            switch (m.srt_type) {
                case Start: {
                    data1[0] = MIDI_BYTE_START;
                    hw.midi.SendMessage(data1, 1);
                    break;
                }
                case Continue: {
                    data1[0] = MIDI_BYTE_CONTINUE;
                    hw.midi.SendMessage(data1, 1);
                    break;
                }
                case Stop: {
                    data1[0] = MIDI_BYTE_STOP;
                    hw.midi.SendMessage(data1, 1);
                    break;
                }
                case TimingClock: {
                    data1[0] = MIDI_BYTE_CLOCK;
                    hw.midi.SendMessage(data1, 1);
                    break;
                }
                
            }            
            break;
        }
    }

}


// Typical Switch case for Message Type.
void Runner::HandleMidiMessage(MidiEvent m)
{

    // will pass it through if it can
    MidiSend(m);

    switch(m.type)
    {
        case NoteOn: {
            NoteOnEvent p = m.AsNoteOn();
            if (p.channel == midiChannel) {
                float velocity = p.velocity / 127.0f;
                if (p.velocity > 0) {
                    if (p.note >= MINIMUM_NOTE && p.note < MINIMUM_NOTE + MIDIMAP_SIZE) {
                        u8 n = p.note - MINIMUM_NOTE;
                        if (kit->midiMap[n] != nullptr) {
                            kit->midiMap[n]->Trigger(velocity);
                            screen.ScreensaveEvent(n);
                            // if (gateOutSource == TRIGGER_ALL_SOURCE || gateOutSource == kit->midiMap[n]) {
                            if (gateOutSource == TRIGGER_ALL_SOURCE) {   // setting it to a specific source doesn't work yet
                                u32 now = System::GetNow();
                                if (now - lastGateOutTime > gateOutLengthMillis) {   // only send one gate if triggers arrive close together
                                    GateOut(true);
                                    lastGateOutTime = now;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case NoteOff: {
            break;
        }
        case ControlChange: {
            ControlChangeEvent p = m.AsControlChange();
            if (p.channel == midiChannel) {
                float sig = (float)p.value / 127;
                if (p.control_number >= MIDICC_VOLUME && p.control_number < MIDICC_VOLUME + MIDICC_LIMIT) {
                    u8 drum = p.control_number - MIDICC_VOLUME - midiCcOffset;
                    mixer.UpdateChannelParam(drum, Channel::PARAM_LEVEL, sig, true);
                }  else if (p.control_number >= MIDICC_SEND_1 && p.control_number < MIDICC_SEND_1 + MIDICC_LIMIT) {
                    u8 drum = p.control_number - MIDICC_SEND_1 - midiCcOffset;
                    mixer.UpdateChannelParam(drum, Channel::PARAM_SEND1, sig, true);
// this doesn't work properly yet & needs some refactoring in the drums
                // }  else if (p.control_number >= MIDICC_PARAM_1 && p.control_number < MIDICC_PARAM_1 + MIDICC_LIMIT) {
                //     u8 drum = p.control_number - MIDICC_PARAM_1 - midiCcOffset;
                //     if (drum < kit->drumCount && kit->drums[drum] != nullptr) {
                //         kit->drums[drum]->UpdateParam(0, sig);
                //     }
                }
            }
            break;
        }
        case ProgramChange: {
            ProgramChangeEvent event = m.AsProgramChange();
            if (event.channel == midiChannel && event.program < PATCH_COUNT) {
                currentPatch = event.program - START_PROGRAM_CHANGE;
                patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, event.program - START_PROGRAM_CHANGE);
                loadFrom = event.program - START_PROGRAM_CHANGE;
            }
            break;
        }
        default: 
            break;
    }
}

void Runner::Run(Kit *kit) {
    if (samplerate == 0) {
        // print error
        return;
    }

    this->kit = kit;

    // Save(0, this->kit);

    // Set up the kit and mixer
    mixer.Reset();
    currentDrum = 0;
    currentMenuIndex = 0;
    mixerSections = kit->drumCount / 4;

    // set level and send for particular drums
    for (u8 drum = 0; drum < kit->drumCount; drum++) {
        float level = 2.4;
        float send1 = 0.0;
        if (kit->drums[drum]->Slot() == "BD" ||
            kit->drums[drum]->Slot() == "SD" ||
            kit->drums[drum]->Slot() == "CH" ||
            kit->drums[drum]->Slot() == "OH") {
                level = 3.0;
        }
        if (kit->drums[drum]->Slot() == "LT" ||
            kit->drums[drum]->Slot() == "MT" ||
            kit->drums[drum]->Slot() == "HT" ||
            kit->drums[drum]->Slot() == "LC" ||
            kit->drums[drum]->Slot() == "HC" ||
            kit->drums[drum]->Slot() == "CB") {
                send1 = 0.8;
        }
        mixer.SetChannelParam(drum, Channel::PARAM_LEVEL, level);
        mixer.SetChannelParam(drum, Channel::PARAM_SEND1, send1);
    }

    // fill the menu
    for (u8 drum = 0; drum < kit->drumCount; drum++) {
        if (kit->drums[drum] != nullptr) {
            screen.menuItems[drum] = kit->drums[drum]->Slot();
        }
    }
    for (u8 i = 0; i < mixerSections; i++) {
        // screen.menuItems[kit->drumCount + i] = "ABCDEFGH"[i];
        screen.menuItems[kit->drumCount + i] = "Mix ";
        screen.menuItems[kit->drumCount + i] += "ABCDEFGH"[i];
    }
    screen.menuItems[kit->drumCount + mixerSections] = "Mix";        // mixer global settings
    screen.menuItems[kit->drumCount + mixerSections + 1] = "Sv/Ld";     // patch storage menu
    menuSize = kit->drumCount + mixerSections + 2;

    // initialize the knob tracking
    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    DrawScreen(true);

    // initially load preset 0
    loadFrom = 0;

    // set up patch save/load
    KitPatch defaultKitPatch;
    SaveToKitPatch(kit, &defaultKitPatch);
    for (u8 i = 0; i < PATCH_COUNT; i++) {
        PersistentStorage<KitPatch> *storage = new PersistentStorage<KitPatch>(hw.seed.qspi);
        savedKits[i] = storage;
        savedKits[i]->Init(defaultKitPatch, sizeof(KitPatch));
    }
    
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
        if (saveTo >= 0 && saveTo < PATCH_COUNT) {
            // Save(saveTo, kit, savedKits[saveTo]);
            saveTo = -1;
        } else if (loadFrom >= 0 && loadFrom < PATCH_COUNT) {
            // Load(loadFrom, kit, savedKits[loadFrom]);
            if (loadFrom < IDRUM_PRESET_COUNT) {       // only using presets for now
                for (u8 drum = 0; drum < kit->drumCount; drum++) {
                    if (kit->drums[drum] != nullptr) {
                        kit->drums[drum]->LoadPreset(loadFrom);
                    }
                }
            }
            loadFrom = -1;
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

        // UpdateFromEncoder();

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
        if (now - lastGateOutTime > gateOutLengthMillis) {
            GateOut(false);
        }

        clock = (clock + 1) % UPDATE_CLOCK_TICKS;
    }
}

void Runner::GlobalAudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    if (globalRunner != nullptr) {
        globalRunner->AudioCallback(in, out, size);
    }
}