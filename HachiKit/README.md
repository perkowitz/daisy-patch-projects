# HachiKit

## Description
An 8-voice virtual analog drumkit.


## Author

Perkowitz

## Overview

HachiKit consists of a set of virtual analog drum sounds that can be triggered via Midi.
Different kit configurations can be created and built. The default kits consist of 8 voices,
but a 16-voice kit can be used if the Patch is run at a lower sampling rate.

The set of voices, clock rate, and assignment of voices to Midi notes is defined in a main Kit file.
Kit-AB and Kit-CD are both 8-voice kits intended to be used together. Kit-ABCD-32k is a 16-voice kit
running at 32k.

To build a kit, use a command like this:

- `make clean; make KIT_NAME=Kit-AB kit`
- `make clean; make KIT_NAME=Kit-CD kit`
- `make clean; make KIT_NAME=Kit-ABCD-32k kit`

And then use `make program-dfu` to load the kit in the Daisy Patch.

## Inputs and Outputs

By default, all voices are output to outputs 1 and 2. Currently, there is no pan control or other stereo processing, so 1 and 2 are identical. Voices can also be
sent to output 3, which can be used as an effect send. On the Mix page, `Lv1`
controls the output to 1 and 2, and `FxA` controls the output to 3.

HachiKit also mixes its outputs 1-3 with the corresponding inputs 1-3, allowing
multiple HachiKit modules to be daisy-chained to a single mixer or effect input.

## Midi Control

HachiKits respond on Midi channel 10, which cannot be edited directly. Note numbers
36-51 are assigned to voices. Note assignments follow a common Midi drum standard.
Kit-AB and Kit-CD are defined with complementary note assignments.

| Note | Kit-AB | Kit-CD |
|---|---|---|
| 36 | Kick     | -             |
| 37 | Rim      | -             |
| 38 | Snare    | -             |
| 39 | Clap     | -             |
| 40 | Snare2   | -             |
| 41 | Lo Tom   | -             |
| 42 | -        | Closed Hat    |
| 43 | Mid Tom  | -             |
| 44 | -        | Shaker        |
| 45 | Hi Tom   | -             |
| 46 | -        | Open Hat      |
| 47 | -        | Lo Perc       |
| 48 | -        | Hi Perc       |
| 49 | -        | Cymbal        |
| 50 | -        | Clave         |
| 51 | -        | Cowbell       |

The volume and send level of each voice can be controlled via Midi continuous controllers. CCs 16-31
control the level of 16 sounds (in the same order as the note assignments), and CCs 32-47
control the `FxA` send. As with note assignments, Kit-AB and Kit-CD have complementary
controller assignments.

Midi messages received at the `Midi In` are copied to the `Midi Out`, allowing HachiKit modules to be daisy-chained.

## Editing Voices and Mix

Each voice has a page where parameters can be edited. Mix parameters also have edit pages, 4 voices 
per page, with an additional Mix output page.

The encoder scrolls through the voice and mixer pages. Some pages have more than 4 editable parameters;
clicking the encoder cycles through additional parameters.
