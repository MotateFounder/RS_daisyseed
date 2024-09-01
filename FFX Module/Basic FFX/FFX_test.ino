#include <Arduino.h>
#include "DaisyDuino.h"

DaisyHardware hw;

#define ALPHA_ENCODER_PIN_A 0
#define ALPHA_ENCODER_PIN_B 1
#define ALPHA_ENCODER_BUTTON_PIN 27

size_t num_channels;

const unsigned long debounceDelay = 25; // Délai réduit pour une meilleure réactivité

volatile int alphaEncoderPos = 0;
volatile int alphaEncoderCount = 0;
int lastAlphaMSB = 0;
int lastAlphaLSB = 0;
const int MAX_ALPHA = 100; // Alpha maximum
int currentAlpha = 50; // Alpha par défaut à 50%

float alpha = 0.5f;  // Déclaration globale de alpha

struct FirstOrderIIR {
    float alpha;
    float out[2];
};

// Initialiser le filtre avec une instance globale
FirstOrderIIR filt = {alpha, {0.0f, 0.0f}};

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    filt.alpha = alpha;  
    float output = (1.0f - filt.alpha) * in[0][i] + filt.alpha * filt.out[0];
    filt.out[0] = output;
    filt.out[1] = output;  // Assuming you want to return the same value for both channels  

    out[0][i] = output;
    out[1][i] = output;
  }
}

void setup() {
  // Initialize seed at 48kHz
  float sample_rate;
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();
  
  pinMode(ALPHA_ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ALPHA_ENCODER_PIN_B, INPUT_PULLUP);
  pinMode(ALPHA_ENCODER_BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ALPHA_ENCODER_PIN_A), updateAlphaEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ALPHA_ENCODER_PIN_B), updateAlphaEncoder, CHANGE);

  // Start callback
  DAISY.begin(MyCallback);

  Serial.begin(9600);
  Serial.println("Setup complete");
}

void loop() {
  //analogReadResolution(16);
  
  unsigned long currentMillis = millis();

  // Gérer alpha
    static unsigned long lastAlphaUpdate = 0;
    if (currentMillis - lastAlphaUpdate >= debounceDelay) {
        if (alphaEncoderPos != 0) {
            currentAlpha = constrain(currentAlpha + alphaEncoderPos, 0, MAX_ALPHA);
            alpha = float(currentAlpha) / MAX_ALPHA;  // Normaliser alpha entre 0.0 et 1.0
            alphaEncoderPos = 0; // Réinitialiser la position de alpha
            lastAlphaUpdate = currentMillis;
        }
    }

  // Simulation d'un petit délai pour éviter de saturer le port série
  delay(1);  // Ajustez ce délai en fonction de la vitesse à laquelle vous voulez afficher les valeurs
}


// Fonction d'interruption pour mettre à jour la position de l'encodeur (alpha)
void updateAlphaEncoder() {
    int MSB = digitalRead(ALPHA_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(ALPHA_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastAlphaMSB << 1) | lastAlphaLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) alphaEncoderCount++;
        else if (encoded == 0b10) alphaEncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) alphaEncoderCount++;
        else if (encoded == 0b00) alphaEncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) alphaEncoderCount++;
        else if (encoded == 0b01) alphaEncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) alphaEncoderCount++;
        else if (encoded == 0b11) alphaEncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (alphaEncoderCount >= 4) {
        alphaEncoderPos++;
        alphaEncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (alphaEncoderCount <= -4) {
        alphaEncoderPos--;
        alphaEncoderCount += 4;
    }

    lastAlphaMSB = MSB;
    lastAlphaLSB = LSB;
}
