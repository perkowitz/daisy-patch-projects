# Avatar

## Description

A framework for building & running synthesizers on the Daisy Patch.

## Author

Perkowitz

## Overview

Avatar consists of multiple synthesizers that share basic modules and the same general user interface.
Avatar can run one synth or two simultaneously on separate MIDI channels. Different synths have
different processing requirements and possible polyphony. Generally the VOICE_COUNT value in the .h
file is the maximum polyphony that the synth can support, but it can be loaded with fewer voices enabled.

See `Avatar.cpp` for examples of how to configure and load various synths. See `/lib/audio` for
details of the specific modules.

Parameters are organized into pages of up to four parameters, which can be edited with the Patch's
four controls. Values will not change until the control is turned past the current value, to avoid
jumps in value. The encoder scrolls through pages. If two synths are loaded, pressing the encoder
switches between them (unless you are on the save/load page)

### MIDI Control

By default, synths run on MIDI channels 7 and 8, which can be edited on the output page. Synths do not respond to CV/Gate
input. 8 MIDI continuous controllers, numbers CC16-CC23, can be used to control sound parameters of each synth. The 
controller range is the same as the range when editing via one of the controls.

### Audio Output

Each synth has parameters for its output levels to the 1-2 pair, output 3, and output 4. For example, 1 and 2
can be used for the first synth, 3 for the second, and 4 as an effects send. No synths are currently stereo,
so outputs 1 and 2 are identical.

Synths do not have save/load of patches yet. However, they do have presets. On the Sv/Ld page, select
a preset (0-7) with Ctrl 1, then press the encoder to load that preset. 

## Synths

- **Toph**: a monophonic 1-oscillator synth with a higher-slope filter.
- **Korra**: a paraphonic 1-oscillator synthesizer. Each voice has its own oscillator, but all voices
share a single filter, allowing greater polyphony. Korra also offers wavefolding and additional modulators.
- **Katara**: a fully polyphonic 1-oscillator synthesizer. Katara maxes out at 4-5 voices of polyphony. Katara is not
up to date with all features in Toph and Korra (such as presets).
- **Asami**: a wavetable oscillator synth. The wavetable oscillator isn't working yet, so Asami is a placeholder.

For Toph, Midi CCs control:
saw level, pulse level, sub level, saw2 level, cutoff frequency, resonance, 
cutoff modulation by envelope, and filter envelope decay.

For Korra, Midi CCs control:
saw level, pulse level, sub level, saw2 level, cutoff frequency, resonance, 
cutoff modulation by envelope, and filter envelope decay.
