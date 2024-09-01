#include "DaisyDuino.h"

Switch button1; 
Switch button2; 

unsigned long long activationTimeButton1 = 0;
unsigned long long activationTimeButton2 = 0;
bool button1Activated = false;
bool button2Activated = false;
bool outputDisplayed = false; // Flag to check if output was already displayed

void setup() {
  Serial.begin(115200); // Augmenter la vitesse de transmission pour gérer les données plus rapides
  
  // Initialisation des boutons
  button1.Init(1, true, 17, INPUT_PULLUP); // Réduisez le délai de debounce à 1 ms
  button2.Init(1, true, 18, INPUT_PULLUP); // Réduisez le délai de debounce à 1 ms
}

float calculateVelocity(unsigned long long delay) {
  const float minDelay = 500.0;  // 0.5 ms
  const float maxDelay = 50000.0; // 50 ms

  if (delay < minDelay) {
    return 1.0; // Vélocité maximale
  } else if (delay > maxDelay) {
    return 0.0; // Vélocité minimale
  } else {
    // Interpolation linéaire entre minDelay et maxDelay
    return 1.0 - (delay - minDelay) / (maxDelay - minDelay);
  }
}

void loop() {
  button1.Debounce();
  button2.Debounce();
  
  // Vérifiez l'état du bouton 1
  if (button1.Pressed()) {
    if (!button1Activated) {
      button1Activated = true;
      activationTimeButton1 = micros(); // Utilisez micros() pour une mesure plus précise
    }
  } else {
    button1Activated = false;
  }

  // Vérifiez l'état du bouton 2
  if (button2.Pressed()) {
    if (!button2Activated) {
      button2Activated = true;
      activationTimeButton2 = micros(); // Utilisez micros() pour une mesure plus précise
    }
  } else {
    button2Activated = false;
  }

  // Si les deux boutons sont activés
  if (button1Activated && button2Activated && !outputDisplayed) {
    unsigned long long delayBetweenActivations = (activationTimeButton2 > activationTimeButton1) 
                                                  ? activationTimeButton2 - activationTimeButton1 
                                                  : activationTimeButton1 - activationTimeButton2;
    
    // Calculer la vélocité
    float velocity = calculateVelocity(delayBetweenActivations);

    // Afficher les délais et la vélocité
    Serial.print("Activation première : ");
    if (activationTimeButton1 < activationTimeButton2) {
      Serial.print("Bouton 1, délai : ");
    } else {
      Serial.print("Bouton 2, délai : ");
    }
    Serial.print(delayBetweenActivations);
    Serial.print(" us, Vélocité : ");
    Serial.print(velocity * 100); // Afficher la vélocité en pourcentage
    Serial.println(" %"); // Afficher en pourcentage
    
    outputDisplayed = true; // Marquer que le message a été affiché
  }

  // Réinitialiser les états lorsque les boutons ne sont plus pressés
  if (!button1.Pressed() && !button2.Pressed()) {
    outputDisplayed = false; // Permettre un nouveau message lorsque les boutons seront pressés à nouveau
  }

  delayMicroseconds(10);  // Petit délai pour la stabilité et pour éviter les lectures trop rapides
}
