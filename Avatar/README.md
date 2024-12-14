# HachiKit

## Description
A 16-voice virtual analog drumkit.


## Author

Perkowitz

## Performance

Basic modules require the following CPU percentages (at 48k, 400mhz):
- Oscillator: 2.3%
- SVF: 1.3%
- WhiteNoise: 0.4%
- AdEnv: 1.3%
- ADSR: 0.8%
- OnePole: 0.6%
- AnalogBassDrum: 21.9%
- AnalogSnareDrum: 87.2%
- SquareNoise: 1.8%
- ZOscillator: 6.0%
- ModalVoice: 47.8%
- Wavefolder: 0.9%
- ClockedNoise: 1.4%
- FormantOscillator: 3.1%


Drum sounds require the following percentages of CPU to run (at 48k, 400mhz):
- ClickSource - 12%
- HhSource68 - 8%
- Bd8 - 
- Ch - 3% (+ HhSource68)
- Cow8 - 
- Cy - 
- FmDrum - 
- Oh - 3% (+ HhSource68)
- Sd8 - 
- SdNoise - 
- Tom - 7% (+ ClickSource)

## MIDI Controllers

HachiKit uses the following MIDI CC ranges for these functions:

- Volume of each drum voice: CC 16-31
- Send A for each drum voice: CC 32-47
- Drum voice parameter 1: CC 48-63
- Drum voice parameter 2: CC 64-79
- Drum voice parameter 3: CC 80-95
- Drum voice parameter 4: CC 96-111
- FX: CC 112-119
