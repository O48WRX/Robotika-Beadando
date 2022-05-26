#ifndef Projektfgv_h
#define Projektfgv_h
#include "Arduino.h"
class Projektfgv {
  public:
    Projektfgv();
    float averageTemp(float temp1,float temp2);
    void manageButtonHold(int pin);
    void manageRGBLED(int pin);
};
#endif
