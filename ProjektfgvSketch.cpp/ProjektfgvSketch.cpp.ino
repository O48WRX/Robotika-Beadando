#include "Arduino.h"
#include "Projektfgv.h"
Projektfgv::Projektfgv() {
  
}
float Projektfgv::averageTemp(float temp1, float temp2) {
  // Visszaadja a két hőmérséklet átlagát.
  return (temp1 + temp2) / 2;
}
bool Projektfgv::isButtonHeld(int pin){
  // Lenyomáskor adjon vissza true-t, ha nincs lenyomva akkor false-t.

  bool wasPushed = false;
  
  if(digitalRead(pin) == HIGH) {
    if(wasPushed == false) {
      wasPushed = true;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}
bool Projektfgv::isItHot(float currentTemp) {
  // Megnézi, hogy a mostani hőmérséklet szerint meleg van-e.

  if(currentTemp > 25) {
    return true;
  } else {
    return false;
  }
}
void Projektfgv::manageRGBLED(float currentTemp, int redPin, int bluePin, int greenPin) {
  // Ha hőmérséklet nagyobb mint az első int, a pirosra ír 255-öt, ha kisebb akkor meg a kékre.
  if(isItHot(currentTemp)) {
    // Magas hőmérséklet
    digitalWrite(bluePin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
  } else {
    // Alacsony hőmérséklet
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
  }
}
