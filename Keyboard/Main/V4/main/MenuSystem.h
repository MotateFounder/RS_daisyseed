#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

class MenuSystem {
public:
    MenuSystem();
    void initialize();
    void setDisplay(Adafruit_SSD1306* disp);
    void setEncoderState(volatile int* encoderPos, volatile int* lastEncoderPos, bool* encoderButtonPressed);
    void displayMenu();
    void handleNavigation(int steps);
    void handleSelection();
    void handleBackNavigation();
    void saveState();
    void restoreState();
    int getCurrentOscillator() const;

    enum MenuState {
        MAIN_MENU,
        OSCILLATOR_MENU,
        FILTER_MENU,
        ENVELOPE_MENU,
        MODULATION_MENU,
        EFFECTS_MENU,
        SYSTEM_SETTINGS_MENU
    };


    MenuState menuState;
    int currentMenuItem;
    int currentOscillator;
    int currentFilter;
    int currentEnvelope;
    int currentModulation;
    int currentEffects;
    int currentSystemSettings;

private:
    void nextMenuItem();
    void previousMenuItem();
    void nextOscillatorType();
    void previousOscillatorType();
    void nextFilterType();
    void previousFilterType();
    void nextEnvelopeType();
    void previousEnvelopeType();
    void nextModulationType();
    void previousModulationType();
    void nextEffectsType();
    void previousEffectsType();
    void nextSystemSetting();
    void previousSystemSetting();

    void selectOscillatorType();
    void selectFilterType();
    void selectEnvelopeType();
    void selectModulationType();
    void selectEffectsType();
    void selectSystemSetting();

    Adafruit_SSD1306* display;
    volatile int* encoderPos;
    volatile int* lastEncoderPos;
    bool* encoderButtonPressed;

    const int menuSize = 6;
    const char* menuItems[6];
    const char* oscillatorTypes[6];
    const char* filterTypes[4];
    const char* envelopeTypes[4];
    const char* modulationTypes[3];
    const char* effectsTypes[3];
    const char* systemSettingsTypes[3];
};

#endif // MENUSYSTEM_H