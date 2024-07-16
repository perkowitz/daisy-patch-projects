// all kit .h files should include this so only one can get loaded
#ifndef DRUMKIT_H
#define DRUMKIT_H

IDrum *drums[1];
u8 drumCount = 1;
DrumWrapper drumWrappers[1];

Bd8 bd;


void InitKit(float samplerate) {

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.1, 4, 0.1, 0.15, 125);

    // Assign sounds to kit
    drums[0] = &bd;

    for (u8 i = 0; i < drumCount; i++) {
        drumWrappers[i].Init(drums[i]);
        drums[i] = &drumWrappers[i];
    }

    drumWrappers[0].setBufferEnabled(false);

}

#endif
