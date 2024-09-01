#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MenuSystem.h"
#include "Receive_audio.h" 
#include "Process_audio.h" 
#include "Output_audio.h" 
#include "bitmaps.h"
#include <DaisyDuino.h>
#include "daisysp.h"        // include the daisy SP lib


using namespace daisysp;    // namespace for SP lib
using namespace daisy;      // namespace for daisy

// Dimensions de l'écran
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Création d'une instance de l'écran OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Pins pour les trimmers gains
#define GIN0_PIN A0
#define GIN1_PIN A1


// Pins pour les boutons 
#define SW_GRPIN_BUTTON_PIN 8
#define SW_GRPOUT_BUTTON_PIN 9


// Pins pour les encodeurs rotatifs
#define ENCODER_PIN_A 13
#define ENCODER_PIN_B 14
#define ENCODER_BUTTON_PIN 10

#define VOLUME_IN0_ENCODER_PIN_A 0
#define VOLUME_IN0_ENCODER_PIN_B 1
#define VOLUME_IN1_ENCODER_PIN_A 2
#define VOLUME_IN1_ENCODER_PIN_B 3
#define VOLUME_OUT0_ENCODER_PIN_A 4
#define VOLUME_OUT0_ENCODER_PIN_B 5
#define VOLUME_OUT1_ENCODER_PIN_A 6
#define VOLUME_OUT1_ENCODER_PIN_B 7

#define FFX1_ENCODER_BUTTON_PIN 17
#define FFX1_ENCODER_PIN_A 18
#define FFX1_ENCODER_PIN_B 19
#define FFX2_ENCODER_BUTTON_PIN 20
#define FFX2_ENCODER_PIN_A 21
#define FFX2_ENCODER_PIN_B 22
#define FFX3_ENCODER_BUTTON_PIN 23
#define FFX3_ENCODER_PIN_A 24
#define FFX3_ENCODER_PIN_B 25
#define FFX4_ENCODER_BUTTON_PIN 26
#define FFX4_ENCODER_PIN_A 27
#define FFX4_ENCODER_PIN_B 28





// Variables de l'encodeur principal
volatile int encoderPos = 0;
volatile int encoderCount = 0;
int lastMSB = 0;
int lastLSB = 0;
const unsigned long debounceDelay = 25; // Délai réduit pour une meilleure réactivité
unsigned long lastEncoderUpdate = 0;
bool buttonPressed = false; // Drapeau pour suivre l'état du bouton

// Variables pour la gestion du bouton
unsigned long buttonPressStart = 0;
unsigned long lastButtonPress = 0;
bool longPressHandled = false; // Drapeau pour savoir si l'appui long a été traité
const unsigned long longPressDuration = 3000; // Durée d'appui long en millisecondes

// Seuils pour la navigation
const int NAVIGATION_THRESHOLD = 1; // Seuil ajusté pour éviter les sauts
const int NAVIGATION_PRECISION = 1; // Nombre de pas à accumuler avant de déplacer le curseur

// Variables pour le volume
volatile int volumeIn0EncoderPos = 0;
volatile int volumeIn0EncoderCount = 0;
int lastIn0VolumeMSB = 0;
int lastIn0VolumeLSB = 0;
const int MAX_In0VOLUME = 100; // Volume maximum
int currentIn0Volume = 50; // Volume par défaut à 50%

volatile int volumeIn1EncoderPos = 0;
volatile int volumeIn1EncoderCount = 0;
int lastIn1VolumeMSB = 0;
int lastIn1VolumeLSB = 0;
const int MAX_In1VOLUME = 100; // Volume maximum
int currentIn1Volume = 50; // Volume par défaut à 50%

volatile int volumeOut0EncoderPos = 0;
volatile int volumeOut0EncoderCount = 0;
int lastOut0VolumeMSB = 0;
int lastOut0VolumeLSB = 0;
const int MAX_Out0VOLUME = 100; // Volume maximum
int currentOut0Volume = 50; // Volume par défaut à 50%

volatile int volumeOut1EncoderPos = 0;
volatile int volumeOut1EncoderCount = 0;
int lastOut1VolumeMSB = 0;
int lastOut1VolumeLSB = 0;
const int MAX_Out1VOLUME = 100; // Volume maximum
int currentOut1Volume = 50; // Volume par défaut à 50%

// Variables pour les ffx
volatile int FFX1EncoderPos = 0;
volatile int FFX1EncoderCount = 0;
int lastFFX1MSB = 0;
int lastFFX1LSB = 0;
const int MAX_FFX1 = 100; // Valeur maximum
int currentFFX1 = 50; // Valeur par défaut à 50%

volatile int FFX2EncoderPos = 0;
volatile int FFX2EncoderCount = 0;
int lastFFX2MSB = 0;
int lastFFX2LSB = 0;
const int MAX_FFX2 = 100; // Valeur maximum
int currentFFX2 = 50; // Valeur par défaut à 50%

volatile int FFX3EncoderPos = 0;
volatile int FFX3EncoderCount = 0;
int lastFFX3MSB = 0;
int lastFFX3LSB = 0;
const int MAX_FFX3 = 100; // Valeur maximum
int currentFFX3 = 50; // Valeur par défaut à 50%

volatile int FFX4EncoderPos = 0;
volatile int FFX4EncoderCount = 0;
int lastFFX4MSB = 0;
int lastFFX4LSB = 0;
const int MAX_FFX4 = 100; // Valeur maximum
int currentFFX4 = 50; // Valeur par défaut à 50%

float Gin0;
float Gin1;

Switch GRPINbutton;
int GRPINState = 0;
Switch GRPOUTbutton;
int GRPOUTState = 0;

float alpha = 0.5f;

// Instanciation du menu
MenuSystem menu;
bool isSleepMode = false; // Variable pour suivre le mode veille

// Instanciation de l'unité d'entrée
Receive_audio rec_audio;

// Instanciation de l'unité de process
Process_audio proc_audio;

// Instanciation de l'unité de sortie
Output_audio out_audio;

DaisyHardware hw;
size_t num_channels;

void InitAudio() {
    // Initialize seed at 48kHz
    float sample_rate;
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    sample_rate = DAISY.get_samplerate();
    // Appel de l'initialisation audio dans MyOscillator
    proc_audio.FirstOrderIIRSetAlpha(&proc_audio.filter, alpha);

    // Start callback
    DAISY.begin(MyCallback);
}


/*  
4 Band EQ for the Daisy Pod, similar to an EQ found in audio outboard gear
params to adjust:
    freq
    gain
    Q TODO (will need a 3rd knob)
*/



// Audio callback function compatible with DAISY.begin
void MyCallback(float** in, float** out, size_t size)
{
    // Process controls and adjust parameters (if needed)
    // NOTE: Placing updateControls() here provides better input response for the encoder/knobs but impacts performance.
    // To improve performance, comment this line out and uncomment the same line in the main while loop.
    // updateControls();

    for (size_t i = 0; i < size; i ++)  
    {
        // Prepare input samples from interleaved input buffer
        float inputSample[2]; // Array to store two input channels
        inputSample[0] = in[0][i]; //* rec_audio.In0volume; // Left channel
        inputSample[1] = in[1][i]; // * rec_audio.In1volume; // Right channel

        // Apply processing to the input sample using a first-order IIR filter
        proc_audio.FirstOrderIIRUpdate(&proc_audio.filter, inputSample);

        // Get the processed output
        float* outputSample = inputSample; //proc_audio.filter.out; // Assuming proc_audio.out stores the latest output samples

        // Write the output samples to the interleaved output buffer
        out[0][i] = in[0][i]; //* out_audio.Out0volume; // Left channel output
        out[1][i] = in[1][i]; //outputSample[1]; //* out_audio.Out1volume; // Right channel output
    }
}




void setup() {
    float sample_rate = DAISY.get_samplerate();
    Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Échec de l'allocation SSD1306"));
        for (;;); // Boucle infinie en cas d'échec d'initialisation
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Initialisez l'oscillateur
    rec_audio.Initialize(&hw, sample_rate); // Assurez-vous que DaisyHardware est correctement initialisé
    proc_audio.Initialize(&hw, sample_rate);
    out_audio.Initialize(&hw, sample_rate);
    InitAudio();

    //proc_audio.InitFreqs();                            // init the freq array
    //proc_audio.InitFilters(sample_rate);                // init the filter array
    //band = 0;                               // set band to 0
    

    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);
    pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);

    pinMode(VOLUME_IN0_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(VOLUME_IN0_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(VOLUME_IN1_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(VOLUME_IN1_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(VOLUME_OUT0_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(VOLUME_OUT0_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(VOLUME_OUT1_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(VOLUME_OUT1_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(FFX1_ENCODER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(FFX1_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(FFX1_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(FFX2_ENCODER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(FFX2_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(FFX2_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(FFX3_ENCODER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(FFX3_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(FFX3_ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(FFX4_ENCODER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(FFX4_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(FFX4_ENCODER_PIN_B, INPUT_PULLUP);

    GRPINbutton.Init(1000, true, SW_GRPIN_BUTTON_PIN, INPUT_PULLUP);
    GRPOUTbutton.Init(1000, true, SW_GRPOUT_BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_IN0_ENCODER_PIN_A), updateVolumeIn0Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_IN0_ENCODER_PIN_B), updateVolumeIn0Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_IN1_ENCODER_PIN_A), updateVolumeIn1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_IN1_ENCODER_PIN_B), updateVolumeIn1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_OUT0_ENCODER_PIN_A), updateVolumeOut0Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_OUT0_ENCODER_PIN_B), updateVolumeOut0Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_OUT1_ENCODER_PIN_A), updateVolumeOut1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(VOLUME_OUT1_ENCODER_PIN_B), updateVolumeOut1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX1_ENCODER_PIN_A), updateFFX1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX1_ENCODER_PIN_B), updateFFX1Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX2_ENCODER_PIN_A), updateFFX2Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX2_ENCODER_PIN_B), updateFFX2Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX3_ENCODER_PIN_A), updateFFX3Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX3_ENCODER_PIN_B), updateFFX3Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX4_ENCODER_PIN_A), updateFFX4Encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FFX4_ENCODER_PIN_B), updateFFX4Encoder, CHANGE);

    menu.setDisplay(&display);
    menu.setEncoderState(&encoderPos, nullptr, nullptr);
    menu.initialize();
    menu.displayMenu();
}

void loop() {
    analogReadResolution(16);

    Gin0 = analogRead(GIN0_PIN) * (3.3 / 65535.0);
    Gin1 = analogRead(GIN1_PIN) * (3.3 / 65535.0);

    GRPINbutton.Debounce();
    GRPOUTbutton.Debounce();


    unsigned long currentMillis = millis();

    // Vérifier si l'encodeur principal a été tourné
    if (currentMillis - lastEncoderUpdate >= debounceDelay) {
        int steps = encoderPos;

        if (abs(steps) >= NAVIGATION_THRESHOLD) {
            // Accumuler les mouvements pour éviter les sauts
            static int accumulatedSteps = 0;
            accumulatedSteps += steps;

            if (abs(accumulatedSteps) >= NAVIGATION_PRECISION) {
                menu.handleNavigation(accumulatedSteps / NAVIGATION_PRECISION); 
                accumulatedSteps = 0; // Réinitialiser le compteur de mouvements
            }

            encoderPos = 0; // Réinitialiser la position de l'encodeur après traitement
            lastEncoderUpdate = currentMillis;
        }
    }

    // Vérifier si l'encodeur FFX1 a été tourné
    static unsigned long lastFFX1Update = 0;
    if (currentMillis - lastFFX1Update >= debounceDelay) {
        if (FFX1EncoderPos != 0) {
            alpha = constrain((float)(currentFFX1 + FFX1EncoderPos), 0.0f, 1.0f);
            proc_audio.FirstOrderIIRSetAlpha(&proc_audio.filter, alpha);
            FFX1EncoderPos = 0; 
            lastFFX1Update = currentMillis;
        }
    }  

    // Gérer les sélections de l'encodeur principal
    static bool lastButtonState = HIGH;
    bool currentButtonState = digitalRead(ENCODER_BUTTON_PIN);

    if (currentButtonState == LOW && lastButtonState == HIGH) {
        delay(20); // Debouncing du bouton réduit pour une meilleure réactivité
        buttonPressed = true; // Marquer le bouton comme pressé
        buttonPressStart = millis(); // Enregistrer l'heure de début de l'appui
        longPressHandled = false; // Réinitialiser le drapeau d'appui long
    } else if (currentButtonState == LOW && buttonPressed) {
        // Si le bouton est maintenu enfoncé
        if (!longPressHandled && (millis() - buttonPressStart >= longPressDuration)) {
            // Traiter l'appui long
            activateSleepMode();
            longPressHandled = true; // Marquer l'appui long comme traité
        }
    } else if (currentButtonState == HIGH && lastButtonState == LOW) {
        // Si le bouton est relâché
        if (buttonPressed && !longPressHandled) {
            if (millis() - lastButtonPress < 500) {
                // Double appui détecté
                menu.handleBackNavigation(); // Traiter le retour en arrière
            } else {
                menu.handleSelection(); // Traiter la sélection si le bouton était pressé
            }
            menu.displayMenu(); // Mettre à jour l'affichage après sélection
        }
        buttonPressed = false; // Réinitialiser le drapeau du bouton
        lastButtonPress = millis(); // Enregistrer le temps du dernier appui
    }
    lastButtonState = currentButtonState;

    // Gérer les groupements entre entrées et sorties
    if (GRPINbutton.Pressed()) {
        GRPINState = 1;
    } else {
        GRPINState = 0; 
    }

    if (GRPOUTbutton.Pressed()) {
        GRPOUTState = 1;
    } else {
        GRPOUTState = 0; 
    }

    // Gérer les volumes
    static unsigned long lastIn0VolumeUpdate = 0;
    if (currentMillis - lastIn0VolumeUpdate >= debounceDelay) {
        if (volumeIn0EncoderPos != 0) {
            currentIn0Volume = constrain(currentIn0Volume + volumeIn0EncoderPos, 0, MAX_In0VOLUME);
            rec_audio.SetIn0Volume(currentIn0Volume);
            volumeIn0EncoderPos = 0; // Réinitialiser la position du volume
            lastIn0VolumeUpdate = currentMillis;
        }
    }

    static unsigned long lastIn1VolumeUpdate = 0;
    if (currentMillis - lastIn1VolumeUpdate >= debounceDelay) {
        if (GRPINState != 0) {
            rec_audio.SetIn1Volume(currentIn0Volume);
            }
        else {
          if (volumeIn1EncoderPos != 0) {
              currentIn1Volume = constrain(currentIn1Volume + volumeIn1EncoderPos, 0, MAX_In1VOLUME);
              rec_audio.SetIn1Volume(currentIn1Volume);
              volumeIn1EncoderPos = 0; // Réinitialiser la position du volume
              lastIn1VolumeUpdate = currentMillis;
          }
        } 
    }

    static unsigned long lastOut0VolumeUpdate = 0;
    if (currentMillis - lastOut0VolumeUpdate >= debounceDelay) {
        if (volumeOut0EncoderPos != 0) {
            currentOut0Volume = constrain(currentOut0Volume + volumeOut0EncoderPos, 0, MAX_Out0VOLUME);
            out_audio.SetOut0Volume(currentOut0Volume);
            volumeOut0EncoderPos = 0; // Réinitialiser la position du volume
            lastOut0VolumeUpdate = currentMillis;
        }
    }

    static unsigned long lastOut1VolumeUpdate = 0;
    if (currentMillis - lastOut1VolumeUpdate >= debounceDelay) {
        if (GRPOUTState != 0) {
            out_audio.SetOut1Volume(currentOut0Volume);
            }
        else {
          if (volumeOut1EncoderPos != 0) {
              currentOut1Volume = constrain(currentOut1Volume + volumeOut1EncoderPos, 0, MAX_Out1VOLUME);
              out_audio.SetOut1Volume(currentOut1Volume);
              volumeOut1EncoderPos = 0; // Réinitialiser la position du volume
              lastOut1VolumeUpdate = currentMillis;
          }
        } 
    }

    // Mettre à jour l'affichage
    updateDisplay();

    //int filtType = menu.getCurrentFilter();
    //proc_audio.UpdateFilter(filtType);
}

// Fonction d'interruption pour mettre à jour la position de l'encodeur principal
void updateEncoder() {
    int MSB = digitalRead(ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastMSB << 1) | lastLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) encoderCount++; // Compter les impulsions
        else if (encoded == 0b10) encoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) encoderCount++;
        else if (encoded == 0b00) encoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) encoderCount++;
        else if (encoded == 0b01) encoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) encoderCount++;
        else if (encoded == 0b11) encoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (encoderCount >= 4) {
        encoderPos++;
        encoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (encoderCount <= -4) {
        encoderPos--;
        encoderCount += 4;
    }

    lastMSB = MSB;
    lastLSB = LSB;
}

// Fonctions d'interruption pour mettre à jour la position des encodeurs volume et ffx
void updateVolumeIn0Encoder() {
    int MSB = digitalRead(VOLUME_IN0_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(VOLUME_IN0_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastIn0VolumeMSB << 1) | lastIn0VolumeLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) volumeIn0EncoderCount++;
        else if (encoded == 0b10) volumeIn0EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) volumeIn0EncoderCount++;
        else if (encoded == 0b00) volumeIn0EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) volumeIn0EncoderCount++;
        else if (encoded == 0b01) volumeIn0EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) volumeIn0EncoderCount++;
        else if (encoded == 0b11) volumeIn0EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (volumeIn0EncoderCount >= 4) {
        volumeIn0EncoderPos++;
        volumeIn0EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (volumeIn0EncoderCount <= -4) {
        volumeIn0EncoderPos--;
        volumeIn0EncoderCount += 4;
    }

    lastIn0VolumeMSB = MSB;
    lastIn0VolumeLSB = LSB;
}

void updateVolumeIn1Encoder() {
    int MSB = digitalRead(VOLUME_IN1_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(VOLUME_IN1_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastIn1VolumeMSB << 1) | lastIn1VolumeLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) volumeIn1EncoderCount++;
        else if (encoded == 0b10) volumeIn1EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) volumeIn1EncoderCount++;
        else if (encoded == 0b00) volumeIn1EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) volumeIn1EncoderCount++;
        else if (encoded == 0b01) volumeIn1EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) volumeIn1EncoderCount++;
        else if (encoded == 0b11) volumeIn1EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (volumeIn1EncoderCount >= 4) {
        volumeIn1EncoderPos++;
        volumeIn1EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (volumeIn1EncoderCount <= -4) {
        volumeIn1EncoderPos--;
        volumeIn1EncoderCount += 4;
    }

    lastIn1VolumeMSB = MSB;
    lastIn1VolumeLSB = LSB;
}

void updateVolumeOut0Encoder() {
    int MSB = digitalRead(VOLUME_OUT0_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(VOLUME_OUT0_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastOut0VolumeMSB << 1) | lastOut0VolumeLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) volumeOut0EncoderCount++;
        else if (encoded == 0b10) volumeOut0EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) volumeOut0EncoderCount++;
        else if (encoded == 0b00) volumeOut0EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) volumeOut0EncoderCount++;
        else if (encoded == 0b01) volumeOut0EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) volumeOut0EncoderCount++;
        else if (encoded == 0b11) volumeOut0EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (volumeOut0EncoderCount >= 4) {
        volumeOut0EncoderPos++;
        volumeOut0EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (volumeOut0EncoderCount <= -4) {
        volumeOut0EncoderPos--;
        volumeOut0EncoderCount += 4;
    }

    lastOut0VolumeMSB = MSB;
    lastOut0VolumeLSB = LSB;
}

void updateVolumeOut1Encoder() {
    int MSB = digitalRead(VOLUME_OUT1_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(VOLUME_OUT1_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastOut1VolumeMSB << 1) | lastOut1VolumeLSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) volumeOut1EncoderCount++;
        else if (encoded == 0b10) volumeOut1EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) volumeOut1EncoderCount++;
        else if (encoded == 0b00) volumeOut1EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) volumeOut1EncoderCount++;
        else if (encoded == 0b01) volumeOut1EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) volumeOut1EncoderCount++;
        else if (encoded == 0b11) volumeOut1EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (volumeOut1EncoderCount >= 4) {
        volumeOut1EncoderPos++;
        volumeOut1EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (volumeOut1EncoderCount <= -4) {
        volumeOut1EncoderPos--;
        volumeOut1EncoderCount += 4;
    }

    lastOut1VolumeMSB = MSB;
    lastOut1VolumeLSB = LSB;
}

void updateFFX1Encoder() {
    int MSB = digitalRead(FFX1_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(FFX1_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastFFX1MSB << 1) | lastFFX1LSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) FFX1EncoderCount++;
        else if (encoded == 0b10) FFX1EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) FFX1EncoderCount++;
        else if (encoded == 0b00) FFX1EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) FFX1EncoderCount++;
        else if (encoded == 0b01) FFX1EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) FFX1EncoderCount++;
        else if (encoded == 0b11) FFX1EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (FFX1EncoderCount >= 4) {
        FFX1EncoderPos++;
        FFX1EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (FFX1EncoderCount <= -4) {
        FFX1EncoderPos--;
        FFX1EncoderCount += 4;
    }

    lastFFX1MSB = MSB;
    lastFFX1LSB = LSB;
}

void updateFFX2Encoder() {
    int MSB = digitalRead(FFX2_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(FFX2_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastFFX2MSB << 1) | lastFFX2LSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) FFX2EncoderCount++;
        else if (encoded == 0b10) FFX2EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) FFX2EncoderCount++;
        else if (encoded == 0b00) FFX2EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) FFX2EncoderCount++;
        else if (encoded == 0b01) FFX2EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) FFX2EncoderCount++;
        else if (encoded == 0b11) FFX2EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (FFX2EncoderCount >= 4) {
        FFX2EncoderPos++;
        FFX2EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (FFX2EncoderCount <= -4) {
        FFX2EncoderPos--;
        FFX2EncoderCount += 4;
    }

    lastFFX2MSB = MSB;
    lastFFX2LSB = LSB;
}

void updateFFX3Encoder() {
    int MSB = digitalRead(FFX3_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(FFX3_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastFFX3MSB << 1) | lastFFX3LSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) FFX3EncoderCount++;
        else if (encoded == 0b10) FFX3EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) FFX3EncoderCount++;
        else if (encoded == 0b00) FFX3EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) FFX3EncoderCount++;
        else if (encoded == 0b01) FFX3EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) FFX3EncoderCount++;
        else if (encoded == 0b11) FFX3EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (FFX3EncoderCount >= 4) {
        FFX3EncoderPos++;
        FFX3EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (FFX3EncoderCount <= -4) {
        FFX3EncoderPos--;
        FFX3EncoderCount += 4;
    }

    lastFFX3MSB = MSB;
    lastFFX3LSB = LSB;
}

void updateFFX4Encoder() {
    int MSB = digitalRead(FFX4_ENCODER_PIN_A); // Most Significant Bit
    int LSB = digitalRead(FFX4_ENCODER_PIN_B); // Least Significant Bit

    int encoded = (MSB << 1) | LSB; // Encode les bits
    int lastEncoded = (lastFFX4MSB << 1) | lastFFX4LSB; // Combine les valeurs des états précédents

    // Comparer les états pour déterminer la direction
    if (lastEncoded == 0b00) {
        if (encoded == 0b01) FFX4EncoderCount++;
        else if (encoded == 0b10) FFX4EncoderCount--;
    } else if (lastEncoded == 0b01) {
        if (encoded == 0b11) FFX4EncoderCount++;
        else if (encoded == 0b00) FFX4EncoderCount--;
    } else if (lastEncoded == 0b11) {
        if (encoded == 0b10) FFX4EncoderCount++;
        else if (encoded == 0b01) FFX4EncoderCount--;
    } else if (lastEncoded == 0b10) {
        if (encoded == 0b00) FFX4EncoderCount++;
        else if (encoded == 0b11) FFX4EncoderCount--;
    }

    // Mettre à jour la position en comptant les impulsions
    if (FFX4EncoderCount >= 4) {
        FFX4EncoderPos++;
        FFX4EncoderCount -= 4; // Compte les impulsions pour un mouvement de crantage complet
    } else if (FFX4EncoderCount <= -4) {
        FFX4EncoderPos--;
        FFX4EncoderCount += 4;
    }

    lastFFX4MSB = MSB;
    lastFFX4LSB = LSB;
}


// Fonction pour activer le mode veille
void activateSleepMode() {
    // Afficher le logo avant de mettre l'écran en veille
    display.clearDisplay();
    display.drawBitmap(0, 0, RS_logo, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();

    delay(2000); // Pause pour que l'image soit visible avant d'éteindre l'écran

    // Mettre en veille l'écran
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    isSleepMode = true; // Activer le mode veille
    //myosc.SetVolume(0);

    // Attendre un appui sur le bouton pour sortir du mode veille
    while (digitalRead(ENCODER_BUTTON_PIN) == HIGH) {
        delay(10); // Petite pause pour éviter une boucle trop rapide
    }

    // Sortir du mode veille
    display.ssd1306_command(SSD1306_DISPLAYON);
    isSleepMode = false; // Désactiver le mode veille

    // Réinitialiser l'affichage et les menus après la sortie du mode veille
    display.clearDisplay();
    display.drawBitmap(0, 0, RS_logo, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();

    delay(2000);
    
    currentIn0Volume = constrain(currentIn0Volume + volumeIn0EncoderPos, 0, MAX_In0VOLUME);
    menu.displayMenu(); // Réafficher le menu
}

// Fonction pour mettre à jour l'affichage
void updateDisplay() {
    display.clearDisplay();

    if (!isSleepMode) {
        // Afficher le menu
        menu.displayMenu();

        // Dessiner la jauge de volume à droite
        int gaugeWidth = 5; // Largeur réduite de moitié
        int gaugeHeight = 50;
        int gaugeX = SCREEN_WIDTH - gaugeWidth - 2; // Position à droite
        int gaugeY = 7; // Position verticale

        int filledHeight = map(currentIn0Volume, 0, MAX_In0VOLUME, 0, gaugeHeight);

        display.drawRect(gaugeX, gaugeY, gaugeWidth, gaugeHeight, SSD1306_WHITE); // Bord de la jauge
        display.fillRect(gaugeX, gaugeY + gaugeHeight - filledHeight, gaugeWidth, filledHeight, SSD1306_WHITE); // Remplissage
    }
    display.display();
}
