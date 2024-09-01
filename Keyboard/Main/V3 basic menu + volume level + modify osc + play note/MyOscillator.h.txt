#ifndef MYOSCILLATOR_H
#define MYOSCILLATOR_H

#include <DaisyDuino.h>
#include <daisysp.h>  // Incluez DaisySP pour les composants audio

using namespace daisysp;  // Utilisez le namespace pour éviter les conflits

class MyOscillator {
public:
    MyOscillator();  

    void Initialize(DaisyHardware* hw);
    void InitializeAudio(float samp_rate);
    void SetVolume(int volume);
    void ProcessAudio(size_t i, float** out, float amplitude, int Trigger);
    void UpdateOscillator(int type);
    void SetFrequency(float frequency);
    
    static const int NUM_OSCILLATOR_TYPES = 6;

private:
    DaisyHardware* hardware;
    float volume;
    // Audio processing components
    Oscillator osc;  // Utilisez le nom correct de la classe
    AdEnv envelope;  // Utilisez le nom correct de la classe
};

#endif // MYOSCILLATOR_H
