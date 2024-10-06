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
            sc = "";
            selected = false;
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
            } else if (currentMenu == MENU_PATCH) {
                if (row == 0) {
                    sc = patchStorage.GetParamSet()->GetParamName(knob);
                    selected = knob == 0;
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
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        std::string sc = "";
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SOUNDS && kit->drums[currentDrum] != nullptr) {
            index = knobRow * KNOB_COUNT + knob;
            sc = kit->drums[currentDrum]->GetParamString(index);
        } else if (currentMenu == MENU_MIXER) {
            index = currentMixerSection * 4 + knob;
            if (index < kit->drumCount && kit->drums[index] != nullptr) {
                sc = mixer.GetChannelParamDisplay(index, knobRow);
            }
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

void Runner::ProcessEncoder() {

    bool redraw = false;
    bool screenOn = false;

    int inc = hw.encoder.Increment();
    if (inc != 0) {
        screenOn = true;
    }
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, menuSize-1);
    // u8 newMenuIndex = (currentMenuIndex + inc) % menuSize-1;
    if (newMenuIndex != currentMenuIndex) {
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
            currentMenu = MENU_PATCH;
            patchStorage.GetParamSet()->ResetParams();
        }

        currentMenuIndex = newMenuIndex;
        redraw = true;
        hw.display.Fill(false);
    }

    if (hw.encoder.RisingEdge()) {
        lastEncoderTime = System::GetNow();
    }
    if (hw.encoder.FallingEdge()) {
        if (System::GetNow() - lastEncoderTime > LONG_PRESS_MILLIS) {
            saveTo = currentPatch;
        } else {
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
            } else if (currentMenu == MENU_PATCH) {
                u8 target = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_TARGET_PATCH);
                u8 operation = (u8)patchStorage.GetParamSet()->GetParamValue(PatchStorage::PARAM_OPERATION);
                switch (operation) {
                    case PatchStorage::OPERATION_LOAD:
                        currentPatch = target;
                        patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, target);
                        loadFrom = target;
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
        } else if (currentMenu == MENU_PATCH) {
            bool changed = patchStorage.GetParamSet()->UpdateParam(knob, sig);
            if (changed) {
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
                            screen.ScreensaveEvent(n);
                        }
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
        case ProgramChange:
        {
            ProgramChangeEvent event = m.AsProgramChange();
            if (event.channel == midiChannel && event.program < PATCH_COUNT) {
                currentPatch = event.program - START_PROGRAM_CHANGE;
                patchStorage.GetParamSet()->SetParam(PatchStorage::PARAM_CURRENT_PATCH, event.program - START_PROGRAM_CHANGE);
                loadFrom = event.program - START_PROGRAM_CHANGE;
                Load(loadFrom, kit, savedKits[loadFrom]);
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
    for (u8 i = 0; i < mixerSections; i++) {
        screen.menuItems[kit->drumCount + i] = "ABCDEFGH"[i];
    }
    screen.menuItems[kit->drumCount + mixerSections] = "Pa";     // patch storage menu
    menuSize = kit->drumCount + mixerSections + 1;

    // initialize the knob tracking
    for (u8 i = 0; i < KNOB_COUNT; i++) {
        lastKnobValue[i] = 0.0f;
    }

    DrawScreen(true);

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
        if (saveTo >= 0 && saveTo < PATCH_COUNT) {
            Save(saveTo, kit, savedKits[saveTo]);
            saveTo = -1;
        } else if (loadFrom >= 0 && loadFrom < PATCH_COUNT) {
            Load(loadFrom, kit, savedKits[loadFrom]);
            loadFrom = -1;
        }

        // usbMidi.Listen();
        // while(usbMidi.HasEvents())
        // {
        //     HandleMidiMessage(usbMidi.PopEvent());
        // }

        DisplayKnobValues();

        if (SHOW_CPU) {
            float avgCpu = meter.GetAvgCpuLoad();
            screen.OledMessage("cpu:" + std::to_string((int)(avgCpu * 100)) + "%", 4, 10);
            screen.ShowCpu(avgCpu);
        }

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