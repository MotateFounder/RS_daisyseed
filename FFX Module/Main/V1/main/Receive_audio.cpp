#include "Receive_audio.h"
#include <cstring> // Pour memcpy

Receive_audio::Receive_audio()
    : hardware(nullptr), sample_rate(0), In0volume(1.0f), In1volume(1.0f)
{
}

void Receive_audio::Initialize(DaisyHardware* hw, float samp_rate) {
    this->hardware = hw;
    this->sample_rate = samp_rate;
}

float* Receive_audio::InAudio(size_t i, float** in) {
    static float output[2]; // Crée un tableau statique pour stocker les résultats

    output[0] = in[0][i] * In0volume;
    output[1] = in[1][i] * In1volume;

    return output;
}

void Receive_audio::SetIn0Volume(int vol) {
    if (vol < 0) {
        vol = 0;
    } else if (vol > 100) {
        vol = 100;
    }
    In0volume = vol / 100.0f;
}

void Receive_audio::SetIn1Volume(int vol) {
    if (vol < 0) {
        vol = 0;
    } else if (vol > 100) {
        vol = 100;
    }
    In1volume = vol / 100.0f;
}
