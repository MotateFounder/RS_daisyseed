#include "Output_audio.h"


Output_audio::Output_audio()
    
{
}

void Output_audio::Initialize(DaisyHardware* hw, float samp_rate) {
    this->hardware = hw;
    sample_rate = samp_rate;
    // Aucune initialisation spécifique nécessaire ici
}

void Output_audio::OutAudio(size_t i, float** out, float* sample) {
    out[0][i] = sample[0]*Out0volume;
    out[1][i] = sample[1]*Out1volume; // Stéréo identique pour cet exemple, ajuster si nécessaire
}

void Output_audio::SetOut0Volume(int vol) {
    // Assurez-vous que vol est compris entre 0 et 100
    if (vol < 0) {
        vol = 0; // Limiter à 0 si en dehors des limites
    } else if (vol > 100) {
        vol = 100; // Limiter à 100 si en dehors des limites
    }
    
    // Convertir int en float et mettre à l'échelle
    Out0volume = vol / 100.0f; // Diviser par 100.0f pour obtenir un float entre 0.0 et 1.0
}

void Output_audio::SetOut1Volume(int vol) {
    // Assurez-vous que vol est compris entre 0 et 100
    if (vol < 0) {
        vol = 0; // Limiter à 0 si en dehors des limites
    } else if (vol > 100) {
        vol = 100; // Limiter à 100 si en dehors des limites
    }
    
    // Convertir int en float et mettre à l'échelle
    Out1volume = vol / 100.0f; // Diviser par 100.0f pour obtenir un float entre 0.0 et 1.0
}