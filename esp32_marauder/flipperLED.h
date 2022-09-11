#ifndef flipperLED_h
#define flipperLED_h

#include <Arduino.h>

#define B_PIN 4
#define G_PIN 5
#define R_PIN 15 //Esp32-S2 mini led pin

class flipperLED {

  public:
    void RunSetup();
    void attackLED();
    void sniffLED();
    void offLED();
};

#endif
