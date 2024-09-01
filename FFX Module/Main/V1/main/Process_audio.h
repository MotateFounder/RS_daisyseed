#ifndef PROCESSAUDIO_H
#define PROCESSAUDIO_H

#include <DaisyDuino.h>
#include <daisysp.h>  // Incluez DaisySP pour les composants audio
#include <vector>

using namespace daisysp;  // Utilisez le namespace pour éviter les conflits

// Définition de FirstOrderIIR comme une structure
struct FirstOrderIIR {
    float alpha;
    float out[2];
};

class Process_audio {
public:
    Process_audio();

    void Initialize(DaisyHardware* hw, float samp_rate);
    void FirstOrderIIRSetAlpha(FirstOrderIIR* filt, float alpha);
    float FirstOrderIIRUpdate(FirstOrderIIR* filt, float* in);

    // Définition des structures pour les buffers interleavés
    struct InterleavingInputBuffer {
        std::vector<float> data;
        size_t numChannels;

        InterleavingInputBuffer(size_t size, size_t channels)
            : data(size * channels, 0.0f), numChannels(channels) {}

        float& operator[](size_t index) {
            return data[index];
        }

        const float& operator[](size_t index) const {
            return data[index];
        }
    };

    struct InterleavingOutputBuffer {
        std::vector<float> data;
        size_t numChannels;

        InterleavingOutputBuffer(size_t size, size_t channels)
            : data(size * channels, 0.0f), numChannels(channels) {}

        float& operator[](size_t index) {
            return data[index];
        }

        const float& operator[](size_t index) const {
            return data[index];
        }
    };

    // Ajoute un membre pour le filtre
    FirstOrderIIR filter;

private:
    DaisyHardware* hardware;
    float sample_rate;

    float minFreq[4];           // array of min freq for each band
    float maxFreq[4];           // array of max freq for each band
    float initFreq[4];          // array of init freq for each band
    int band;                   // the current freq band

    // Structure pour les filtres
    struct Filter {
        Svf filt;       // use the Svf filter lib
        float amp;      // var for amp (gain) value

        void Init(float samplerate, float freq) {
            filt.Init(samplerate);
            filt.SetRes(0.85);
            filt.SetDrive(.002);
            filt.SetFreq(freq);
            amp = .5f;
        }

        float Process(float in) {
            filt.Process(in);
            return filt.Peak() * amp;
        }

        void SetFreq(float targetFreq) {
            filt.SetFreq(targetFreq);
        }
    };

    Filter filters[4];  // create an array to store 4 filters, 1 for each of the 4 bands

    // Methods to initialize frequencies and filters
    void InitFreqs();
    void InitFilters(float samplerate);
    void UpdateFilter(int type);
};

#endif // PROCESSAUDIO_H
