# Audio Library

Contains various modules for creating, processing, and modulating audio.

## Modules

- AdsrEnv: a basic ADSR envelope.
- AhdEnv: an attack/hold/decay envelope that takes a trigger rather than a gate.
- Channel: one channel in a mixer, with level and effects send.
- Drift: a random-sounding modulator where the values drift over time.
- Folding: various wavefolding-like processing functions.
- Mixer: combines channels into a multi-channel audio mixer.
- MultiOsc: a Roland-style oscillator that offers simultaneous saw, pulse,
sub (square wave an octave down) and high (saw wave an octave up) waves.
- Processing: some general audio-processing functions.
- SyncEnv: an envelope that syncs to a clock rather than being triggered on every note.
- SyncMultiOsc: a MultiOsc that also implements oscillator sync.
- WaveSyncOsc: an oscillator that uses oscillator sync to create a wavetable-like effect
from a wav file.