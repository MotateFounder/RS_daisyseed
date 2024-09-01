#include "MyOscillator.h"

MyOscillator::MyOscillator()
    : volume(0.5f) // Volume initial à 50%
{
    // Ne pas tenter d'initialiser currentOscillator ici
}

void MyOscillator::Initialize(DaisyHardware* hw) {
    this->hardware = hw;
    // Aucune initialisation spécifique nécessaire ici
}

void MyOscillator::InitializeAudio(float samp_rate) {
    if (hardware) {
        osc.Init(samp_rate);  // Passez le taux d'échantillonnage depuis le matériel
        envelope.Init(samp_rate);  // Passez le taux d'échantillonnage depuis le matériel
        envelope.SetTime(0, 0.1f);  // Ajustez la méthode pour la durée
        envelope.SetTime(ADENV_SEG_ATTACK, 0.05);
        envelope.SetTime(ADENV_SEG_DECAY, 0.5);
        envelope.SetMin(0);
        envelope.SetMax(1.0);
        envelope.SetCurve(0);
        UpdateOscillator(0);
    }
}


void MyOscillator::SetVolume(int vol) {
    // Assurez-vous que vol est compris entre 0 et 100
    if (vol < 0) {
        vol = 0; // Limiter à 0 si en dehors des limites
    } else if (vol > 100) {
        vol = 100; // Limiter à 100 si en dehors des limites
    }
    
    // Convertir int en float et mettre à l'échelle
    volume = vol / 100.0f; // Diviser par 100.0f pour obtenir un float entre 0.0 et 1.0
}


void MyOscillator::ProcessAudio(size_t i, float** out, float amplitude, int Trigger) {
    float env_value = envelope.Process();
    osc.SetAmp(amplitude * volume * env_value);
    if (Trigger == 1) {
      envelope.Trigger();
    }
    float sample = osc.Process();
    out[0][i] = sample;
    out[1][i] = sample; // Stéréo identique pour cet exemple, ajuster si nécessaire
}



void MyOscillator::UpdateOscillator(int type) {
    switch (type) {
        case 0: osc.SetWaveform(osc.WAVE_SIN); break;
        case 1: osc.SetWaveform(osc.WAVE_TRI); break;
        case 2: osc.SetWaveform(osc.WAVE_SQUARE); break;
        case 3: osc.SetWaveform(osc.WAVE_SAW); break;
        case 4: 
            // Implémentation pour bruit blanc (ajoutez votre code ici)
            break;
        case 5: 
            // Implémentation pour bruit rose (ajoutez votre code ici)
            break;
        default:
            // Gestion des types d'ondes invalides
            break;
    }
}

void MyOscillator::SetFrequency(float frequency) {
    osc.SetFreq(frequency);
}
