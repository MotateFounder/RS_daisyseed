#include "MenuSystem.h"

MenuSystem::MenuSystem()
    : menuItems{"Oscillator", "Filter", "Envelope", "Modulation", "Effects", "System Settings"},
      oscillatorTypes{"Sine", "Triangle", "Square", "Saw", "White Noise", "Pink Noise"},
      filterTypes{"Low Pass", "High Pass", "Band Pass", "Notch"},
      envelopeTypes{"Envelope 1", "Envelope 2", "Envelope 3", "Envelope 4"},
      modulationTypes{"Modulation 1", "Modulation 2", "Modulation 3"},
      effectsTypes{"Effect 1", "Effect 2", "Effect 3"},
      systemSettingsTypes{"System Setting 1", "System Setting 2", "System Setting 3"},
      currentMenuItem(0),
      currentOscillator(0),
      currentFilter(0),
      currentEnvelope(0),
      currentModulation(0),
      currentEffects(0),
      currentSystemSettings(0),
      menuState(MAIN_MENU)
{}


void MenuSystem::initialize() {
    // Initialisation des éléments de menu si nécessaire
}

void MenuSystem::setDisplay(Adafruit_SSD1306* disp) {
    display = disp;
}

void MenuSystem::setEncoderState(volatile int* encoderPos, volatile int* lastEncoderPos, bool* encoderButtonPressed) {
    this->encoderPos = encoderPos;
    this->lastEncoderPos = lastEncoderPos;
    this->encoderButtonPressed = encoderButtonPressed;
}

void MenuSystem::displayMenu() {
    display->clearDisplay();

    switch (menuState) {
        case MAIN_MENU:
            for (int i = 0; i < menuSize; i++) {
                display->setCursor(0, i * 10);
                if (i == currentMenuItem) {
                    display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
                    display->print(">");
                } else {
                    display->setTextColor(SSD1306_WHITE);
                }
                display->print(menuItems[i]);
            }
            break;

        case OSCILLATOR_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("Oscillator Type: ");
            display->print(oscillatorTypes[currentOscillator]);
            break;

        case FILTER_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("Filter Type: ");
            display->print(filterTypes[currentFilter]);
            break;

        case ENVELOPE_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("Envelope Type: ");
            display->print(envelopeTypes[currentEnvelope]);
            break;

        case MODULATION_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("Modulation Type: ");
            display->print(modulationTypes[currentModulation]);
            break;

        case EFFECTS_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("Effects Type: ");
            display->print(effectsTypes[currentEffects]);
            break;

        case SYSTEM_SETTINGS_MENU:
            display->setCursor(0, 0);
            display->setTextColor(SSD1306_WHITE);
            display->print("System Setting: ");
            display->print(systemSettingsTypes[currentSystemSettings]);
            break;
    }
    display->display();
}

void MenuSystem::handleNavigation(int steps) {
    if (steps == 0) return; // Ignorer si aucun changement

    if (menuState == MAIN_MENU) {
        if (steps > 0) {
            nextMenuItem(); // Sens horaire : faire défiler vers le bas
        } else {
            previousMenuItem(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == OSCILLATOR_MENU) {
        if (steps > 0) {
            nextOscillatorType(); // Sens horaire : faire défiler vers le bas
        } else {
            previousOscillatorType(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == FILTER_MENU) {
        if (steps > 0) {
            nextFilterType(); // Sens horaire : faire défiler vers le bas
        } else {
            previousFilterType(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == ENVELOPE_MENU) {
        if (steps > 0) {
            nextEnvelopeType(); // Sens horaire : faire défiler vers le bas
        } else {
            previousEnvelopeType(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == MODULATION_MENU) {
        if (steps > 0) {
            nextModulationType(); // Sens horaire : faire défiler vers le bas
        } else {
            previousModulationType(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == EFFECTS_MENU) {
        if (steps > 0) {
            nextEffectsType(); // Sens horaire : faire défiler vers le bas
        } else {
            previousEffectsType(); // Sens antihoraire : faire défiler vers le haut
        }
    } else if (menuState == SYSTEM_SETTINGS_MENU) {
        if (steps > 0) {
            nextSystemSetting(); // Sens horaire : faire défiler vers le bas
        } else {
            previousSystemSetting(); // Sens antihoraire : faire défiler vers le haut
        }
    }
}




void MenuSystem::handleSelection() {
    if (menuState == MAIN_MENU) {
        switch (currentMenuItem) {
            case 0: selectOscillatorType(); break;
            case 1: selectFilterType(); break;
            case 2: selectEnvelopeType(); break;
            case 3: selectModulationType(); break;
            case 4: selectEffectsType(); break;
            case 5: selectSystemSetting(); break;
        }
    }
    // Les sélections pour les autres menus ne sont pas spécifiées ici
}

void MenuSystem::handleBackNavigation() {
    if (menuState != MAIN_MENU) {
        menuState = MAIN_MENU;
        displayMenu();
    }
}

void MenuSystem::nextMenuItem() {
    currentMenuItem = (currentMenuItem + 1) % menuSize;
    displayMenu();
}

void MenuSystem::previousMenuItem() {
    currentMenuItem = (currentMenuItem - 1 + menuSize) % menuSize;
    displayMenu();
}

void MenuSystem::nextOscillatorType() {
    currentOscillator = (currentOscillator + 1) % 6;
    displayMenu();
}

void MenuSystem::previousOscillatorType() {
    currentOscillator = (currentOscillator - 1 + 6) % 6;
    displayMenu();
}

int MenuSystem::getCurrentOscillator() const {
    return currentOscillator;
}

void MenuSystem::nextFilterType() {
    currentFilter = (currentFilter + 1) % 4;
    displayMenu();
}

void MenuSystem::previousFilterType() {
    currentFilter = (currentFilter - 1 + 4) % 4;
    displayMenu();
}

int MenuSystem::getCurrentFilter() const {
    return currentFilter;
}

void MenuSystem::nextEnvelopeType() {
    currentEnvelope = (currentEnvelope + 1) % 4;
    displayMenu();
}

void MenuSystem::previousEnvelopeType() {
    currentEnvelope = (currentEnvelope - 1 + 4) % 4;
    displayMenu();
}

void MenuSystem::nextModulationType() {
    currentModulation = (currentModulation + 1) % 3;
    displayMenu();
}

void MenuSystem::previousModulationType() {
    currentModulation = (currentModulation - 1 + 3) % 3;
    displayMenu();
}

void MenuSystem::nextEffectsType() {
    currentEffects = (currentEffects + 1) % 3;
    displayMenu();
}

void MenuSystem::previousEffectsType() {
    currentEffects = (currentEffects - 1 + 3) % 3;
    displayMenu();
}

void MenuSystem::nextSystemSetting() {
    currentSystemSettings = (currentSystemSettings + 1) % 3;
    displayMenu();
}

void MenuSystem::previousSystemSetting() {
    currentSystemSettings = (currentSystemSettings - 1 + 3) % 3;
    displayMenu();
}

void MenuSystem::selectOscillatorType() {
    menuState = OSCILLATOR_MENU;
    displayMenu();
}

void MenuSystem::selectFilterType() {
    menuState = FILTER_MENU;
    displayMenu();
}

void MenuSystem::selectEnvelopeType() {
    menuState = ENVELOPE_MENU;
    displayMenu();
}

void MenuSystem::selectModulationType() {
    menuState = MODULATION_MENU;
    displayMenu();
}

void MenuSystem::selectEffectsType() {
    menuState = EFFECTS_MENU;
    displayMenu();
}

void MenuSystem::selectSystemSetting() {
    menuState = SYSTEM_SETTINGS_MENU;
    displayMenu();
}

void MenuSystem::saveState() {
    /* Code pour sauvegarder l'état actuel */;
}

void MenuSystem::restoreState() {
    /* Code pour restaurer l'état sauvegardé */;
}