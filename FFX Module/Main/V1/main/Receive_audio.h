#ifndef RECEIVEAUDIO_H
#define RECEIVEAUDIO_H

#include <DaisyDuino.h>
#include <daisysp.h>  // Incluez DaisySP pour les composants audio

using namespace daisysp;  // Utilisez le namespace pour éviter les conflits

class Receive_audio {
public:
    Receive_audio();

    void Initialize(DaisyHardware* hw, float samp_rate);
    float* InAudio(size_t i, float** in);  // Modification ici
    void SetIn0Volume(int vol);
    void SetIn1Volume(int vol);

    float In0volume;
    float In1volume;

private:
    DaisyHardware* hardware;
    float sample_rate;
};

#endif // RECEIVEAUDIO_H
