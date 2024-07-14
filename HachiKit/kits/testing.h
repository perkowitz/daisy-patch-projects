
IDrum *drums[1];
u8 drumCount = 1;
DrumWrapper drumWrappers[1];

Bd8 bd;


void InitKit(float samplerate) {

    // Init all drum sounds
    bd.Init("BD", samplerate, 64, 0.001, 4, 0.001, 0.15, 125);

    // Assign sounds to kit
    drums[0] = &bd;

    for (u8 i = 0; i < drumCount; i++) {
        drumWrappers[i].Init(drums[i]);
        drums[i] = &drumWrappers[i];
    }

    // drumWrappers[0].setBufferEnabled(false);

}
