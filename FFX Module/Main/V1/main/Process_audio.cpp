#include "Process_audio.h"

Process_audio::Process_audio() {
    // Initialisation du filtre
    filter.alpha = 0.0f;
    filter.out[0] = 0.0f;
    filter.out[1] = 0.0f;
}

void Process_audio::Initialize(DaisyHardware* hw, float samp_rate) {
    this->hardware = hw;
    sample_rate = samp_rate;
}

void Process_audio::FirstOrderIIRSetAlpha(FirstOrderIIR* filt, float alpha) {
    if (alpha < 0.0f) {
        filt->alpha = 0.0f;
    } else if (alpha > 1.0f) {
        filt->alpha = 1.0f;
    } else {
        filt->alpha = alpha;
    }

    filt->out[0] = 0.0f;
    filt->out[1] = 0.0f;
}

float Process_audio::FirstOrderIIRUpdate(FirstOrderIIR* filt, float* in) {
    float output = (1.0f - filt->alpha) * in[0] + filt->alpha * filt->out[0];
    filt->out[0] = output;
    filt->out[1] = output;  // Assuming you want to return the same value for both channels

    return output;
}

void Process_audio::InitFreqs() {
    minFreq[0] = 30;
    initFreq[0] = 200;
    maxFreq[0] = 450;
    minFreq[1] = 200;
    initFreq[1] = 1000;
    maxFreq[1] = 2500;
    minFreq[2] = 600;
    initFreq[2] = 3500;
    maxFreq[2] = 7500;
    minFreq[3] = 1500;
    initFreq[3] = 7500;
    maxFreq[3] = 16000;  // MAX sample rate / 3 = 48k/3 = 16khz
}

void Process_audio::InitFilters(float samplerate) {
    for (int i = 0; i < 4; i++) {
        filters[i].Init(samplerate, initFreq[i]);
    }
}

// Dummy implementation for the UpdateFilter method
void Process_audio::UpdateFilter(int type) {
    // Implementation needed for updating filter types based on 'type' parameter
}
