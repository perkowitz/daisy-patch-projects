#include "Mixer16.h"

using namespace daisy;
using namespace daisysp;

void Mixer16::Reset() {
    for (u8 channel = 0; channel < CHANNELS; channel++) {
        channels[channel].level = 1;
        channels[channel].pan = 0;
        channels[channel].send1 = 0;
        channels[channel].send2 = 0;
    }

    ResetSignals();
}

void Mixer16::ResetSignals() {
    for (u8 channel = 0; channel < CHANNELS; channel++) {
        channels[channel].signal = 0;
    }

    leftSignal = rightSignal = send1Signal = send2Signal = 0;
}

void Mixer16::Process() {
    leftSignal = Utility::LimitFloat(outputGain * leftSignal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    rightSignal = Utility::LimitFloat(outputGain * rightSignal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    send1Signal = Utility::LimitFloat(send1Gain * send1Signal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    send2Signal = Utility::LimitFloat(send2Gain * send2Signal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
}

SimpleChannel* Mixer16::GetChannel(u8 channel) {
    if (channel >= CHANNELS) return nullptr;

    return &channels[channel];
}

void Mixer16::UpdateSignal(u8 channel, float signal) {
    if (channel >= CHANNELS) return;

    channels[channel].signal = signal;
    leftSignal += signal * channels[channel].level;    // no panning
    rightSignal += signal * channels[channel].level;
    send1Signal += signal * channels[channel].send1;
    send2Signal += signal * channels[channel].send2;
}

void Mixer16::SetLevel(u8 channel, float level) {
    if (channel >= CHANNELS) return;

    channels[channel].level = Utility::LimitFloat(level, 0, 2);
}

void Mixer16::SetPan(u8 channel, float pan) {
    if (channel >= CHANNELS) return;

    channels[channel].pan = Utility::LimitFloat(pan, -1, 1);
}

void Mixer16::SetSend1(u8 channel, float level) {
    if (channel >= CHANNELS) return;

    channels[channel].send1 = Utility::LimitFloat(level, 0, 1);;
}

void Mixer16::SetSend2(u8 channel, float level) {
    if (channel >= CHANNELS) return;

    channels[channel].send2 = Utility::LimitFloat(level, 0, 1);
}

