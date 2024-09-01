#ifndef OUTPUTAUDIO_H
#define OUTPUTAUDIO_H

#include <DaisyDuino.h>
#include <daisysp.h>  // Incluez DaisySP pour les composants audio

using namespace daisysp;  // Utilisez le namespace pour éviter les conflits

class Output_audio {
public:
    Output_audio();

    void Initialize(DaisyHardware* hw, float samp_rate);
    void OutAudio(size_t i, float** out, float* sample);
    void SetOut0Volume(int vol);
    void SetOut1Volume(int vol);

    float Out0volume;
    float Out1volume;

private:
    DaisyHardware* hardware;
    float sample_rate;
    
};

#endif // OUTPUTAUDIO_H
