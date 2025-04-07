#include "Runner.h"
#include "Korra.h"

using namespace daisy;
using namespace daisysp;

Runner* Runner::globalRunner = nullptr;

// Display the available parameter names.
void Runner::DisplayParamMenu() {

    // display labels for the parameters edited by the knobs
    std::string sc = "";
    std::string rc = "";
    ParamPage *paramPage = currentSynth->GetParamPage(currentSynthPage);
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        sc = "";
        rc = "";
        if (currentMenu == MENU_SYNTH && currentSynthPage < currentSynth->PageCount()) {
            sc = paramPage->GetParamName(knob);
        }
        Rectangle rect2(knob * 32, 12, 31, 12);
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

    u8 index;
    ParamPage *paramPage = currentSynth->GetParamPage(currentSynthPage);
    for (int knob = 0; knob < KNOB_COUNT; knob++) {
        std::string sc = "";
        Rectangle rect(knob * 32, 0, 32, 8);
        if (currentMenu == MENU_SYNTH && currentSynthPage < currentSynth->PageCount()) {
            sc = paramPage->GetParamDisplay(knob);
        }
        screen.WriteStringAligned(sc.c_str(), Font_6x8, rect, Alignment::centered, true);
    }

    // Korra *korra = (Korra*)currentSynth;
    // screen.DrawRect(0, 31, 127, 63, false, true);
    // for (u8 step = 0; step < 8; step++) {
    //     u8 value = (int)(korra->GetDrift(step) * 32);
    //     screen.DrawLine(step * 16, 63 - value, step * 16 + 8, 63 - value, true);
    // }

}

void Runner::DrawScreen(bool clearFirst) {
    if (clearFirst) {
        hw.display.Fill(false);
    }
    screen.OledMessage("Draw0", 0);
    // screen.DrawMenu(currentMenuIndex);
    DisplayParamMenu();
    DisplayKnobValues();
    hw.display.Update();        
}

void Runner::ProcessEncoder() {

    bool screenOn = false;

    // Encoder turn
    int inc = hw.encoder.Increment();
    if (inc != 0) {
        screenOn = true;
    }
    menuSize = currentSynth->PageCount();
    u8 newMenuIndex = Utility::LimitInt(currentMenuIndex + inc, 0, menuSize-1);
    if (newMenuIndex != currentMenuIndex && screen.IsScreenOn()) {
        currentSynthPage = newMenuIndex;
        if (currentSynth == synth1) {
            synth1Page = currentSynthPage;
        } else if (currentSynth == synth2) {
            synth2Page = currentSynthPage;
        }
        currentSynth->GetParamPage(currentSynthPage)->ResetParams();
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
            if (currentSynth == synth1 && synth2 != nullptr) {
                currentSynth = synth2;
                currentSynthPage = currentMenuIndex = synth2Page;
                redraw = true;
            } else if (currentSynth == synth2) {
                currentSynth = synth1;
                currentSynthPage = currentMenuIndex = synth1Page;
                redraw = true;
            }
            currentSynth->GetParamPage(currentSynthPage)->ResetParams();
        }
    }

    if (screenOn) {
        lastScreenTime = System::GetNow();
        if (!screen.IsScreenOn()) {
            screen.SetScreenOn(true);
            redraw = true;  // redraw will happen in the outer loop, not here in the audio loop
        }
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
            changed = currentSynth->GetParamPage(currentSynthPage)->UpdateParam(knob, sig);
        }
        if (changed) {
            lastKnobValue[knob] = sig;
            anyChanged = true;
        }
    }

    if (anyChanged) {
        currentSynth->ProcessChanges();
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
        //     float signal = synths[currentSynthIndex]->Process();
        //     mixer.ResetSignals();
        //     mixer.UpdateSignal(0, signal);
        // // }
        synth1->Process();
        if (synth2 != nullptr) {
            synth2->Process();
        }

        for (u8 o = 0; o < 4; o++) {
            out[o][i] = synth1->GetOutput(o);
            if (synth2 != nullptr) {
                out[o][i] += synth2->GetOutput(o);
            }
        }

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

void Runner::HandleMidiRealtime(MidiEvent m) {

    switch (m.srt_type) {
        case Start: {
            currentMeasure = currentStep = currentTick = 0;
            running = true;
            break;
        }
        case Continue: {
            currentMeasure = currentStep = currentTick = 0;
            running = true;
            break;
        }
        case Stop: {
            currentMeasure = currentStep = currentTick = 0;
            running = false;
            break;
        }
        case TimingClock: {
            if (currentTick == 0) {
                synth1->Clock(currentMeasure, currentStep, currentTick);
                if (synth2 != nullptr) {
                    synth2->Clock(currentMeasure, currentStep, currentTick);
                }
            }
            currentTick = (currentTick + 1) % TICKS_PER_STEP;
            if (currentTick == 0) {
                currentStep = (currentStep + 1) % STEPS_PER_MEASURE;
                if (currentStep == 0) {
                    currentMeasure++;
                }
            }
            break;
        }
        
    }
}

void Runner::HandleMidiNote(ISynth *synth, u8 note, u8 velocity) {
    if (velocity > 0) {
        float v = (float)velocity / 127.0f;
        synth->NoteOn(note, v);
        screen.ScreensaveEvent(note, true);
    } else {
        synth->NoteOff(note);
        screen.ScreensaveEvent(note, false);
    }
}

// Typical Switch case for Message Type.
void Runner::HandleMidiMessage(MidiEvent m) {

    MidiSend(m);

    switch(m.type) {
        case NoteOn: {
            NoteOnEvent event = m.AsNoteOn();
            // screen.OledMessage("Note:" + std::to_string(event.channel) + ":" + std::to_string(event.note), 3);
            if (event.channel == synth1->GetMidiChannel()) {
                HandleMidiNote(synth1, event.note, event.velocity);
            }
            if (synth2 != nullptr && event.channel == synth2->GetMidiChannel()) {
                HandleMidiNote(synth2, event.note, event.velocity);
            }
            break;
        }
        case NoteOff: {
            NoteOffEvent event = m.AsNoteOff();
            if (event.channel == synth1->GetMidiChannel()) {
                synth1->NoteOff(event.note);
            }
            if (synth2 != nullptr && event.channel == synth2->GetMidiChannel()) {
                synth2->NoteOff(event.note);
            }
            screen.ScreensaveEvent(event.note, false);
            break;
        }
        case ControlChange: {
            ControlChangeEvent event = m.AsControlChange();
            // screen.OledMessage("Ct:" + std::to_string(event.channel))
            // float sig = (float)event.value / 127;
            if (event.channel == synth1->GetMidiChannel()) {
                synth1->MidiController(event.control_number, event.value);
            }
            if (synth2 != nullptr && event.channel == synth2->GetMidiChannel()) {
                synth2->MidiController(event.control_number, event.value);
            }

            // send to synth
            break;
        }
        case ProgramChange: {
            break;
        }
        case SystemRealTime: {
            HandleMidiRealtime(m);
            break;
        }
        default: 
            break;
    }
}

void Runner::Run(ISynth *synth1, ISynth *synth2) {
    if (samplerate == 0) {
        // print error
        return;
    }

    this->synth1 = synth1;
    this->synth2 = synth2;
    currentSynth = this->synth1;
    menuSize = synth1->PageCount();
    if (synth2 != nullptr) {
        menuSize += synth2->PageCount();
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
    hw.SetAudioBlockSize(64);
    hw.midi.StartReceive();
    // usbMidi.StartReceive();
    hw.StartAdc();
    Runner::globalRunner = this;
    hw.StartAudio(GlobalAudioCallback);  

    u32 clock = 0;
    for(;;)
    {
        if (redraw) {
            DrawScreen(true);
            hw.display.Update();        
            redraw = false;
        }

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
            screen.Write(std::to_string((int)(avgCpu * 100)) + "%", 100, 36);
            // screen.ShowCpu(avgCpu, false);
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