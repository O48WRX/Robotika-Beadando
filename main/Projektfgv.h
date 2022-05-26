#ifndef Projektfgv_h
#define Projektfgv_h
#include "Arduino.h"
class Projektfgv {
  public:
    Projektfgv();
    float averageTemp(float temp1,float temp2);
    bool isButtonHeld(int pin);
    bool isItHot(float currentTemp);
    void manageRGBLED(float currentTemp, int redPin, int bluePin, int greenPin);
};
#endif
