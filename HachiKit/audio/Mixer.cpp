#include "Mixer.h"

using namespace daisy;
using namespace daisysp;

void Mixer::Reset() {
    for (u8 channel = 0; channel < CHANNELS; channel++) {
        channels[channel].GetParamSet()->SetParam(Channel::PARAM_LEVEL, 1);
        channels[channel].GetParamSet()->SetParam(Channel::PARAM_PAN, 0);
        channels[channel].GetParamSet()->SetParam(Channel::PARAM_SEND1, 0);
        channels[channel].GetParamSet()->SetParam(Channel::PARAM_SEND2, 0);
    }

    ResetSignals();
}

void Mixer::ResetSignals() {
    for (u8 channel = 0; channel < CHANNELS; channel++) {
        channels[channel].SetSignal(0);
    }

    leftSignal = rightSignal = send1Signal = send2Signal = 0;
}

void Mixer::Process() {
    leftSignal = Utility::LimitFloat(outputGain * leftSignal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    rightSignal = Utility::LimitFloat(outputGain * rightSignal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    send1Signal = Utility::LimitFloat(send1Gain * send1Signal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
    send2Signal = Utility::LimitFloat(send2Gain * send2Signal / CHANNELS, -1 * OUTPUT_LIMIT, OUTPUT_LIMIT);
}

void Mixer::UpdateSignal(u8 channel, float signal) {
    if (channel >= CHANNELS) return;

    channels[channel].SetSignal(signal);
    leftSignal += signal * channels[channel].Level();    // no panning
    rightSignal += signal * channels[channel].Level();
    send1Signal += signal * channels[channel].Send1();
    send2Signal += signal * channels[channel].Send2();
}

bool Mixer::UpdateChannelParam(u8 channel, u8 param, float raw) {
    if (channel < CHANNELS) return false;

    return channels[channel].GetParamSet()->UpdateParam(param, raw);
}

void Mixer::SetChannelParam(u8 channel, u8 param, float scaled) {
    if (channel < CHANNELS) return;

    channels[channel].GetParamSet()->SetParam(param, scaled);
}

std::string Mixer::GetChannelParamDisplay(u8 channel, u8 param) {
    if (channel < CHANNELS) return "";

    return channels[channel].GetParamSet()->GetParamDisplay(param);
}

std::string Mixer::GetChannelParamName(u8 channel, u8 param) {
    if (channel < CHANNELS) return "";

    return channels[channel].GetParamSet()->GetParamName(param);
}

