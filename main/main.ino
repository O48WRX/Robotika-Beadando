#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Projektfgv.h"

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS 5
#define TEMPERATURE_PRECISION 9
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

#define MOTOR 6


#define BUTTON_Y 7
#define BUTTON_R 8

int tempeatureHumidity = 1;
bool ventilator = false;
bool wasPushed1 = false;
bool wasPushed2 = false;
  
Projektfgv projektfgv();

LiquidCrystal_I2C lcd(0x27,16,2);

void setup()
{
  pinMode(5,INPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  dht.begin();
  lcd.init();                      
  lcd.init();
  lcd.backlight();
  
}

float averageTemp(float temp1, float temp2) {
  // Visszaadja a két hőmérséklet átlagát.
  return (temp1 + temp2) / 2;
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

float GetDallasTemp()
{
  for(int i=0;i<sensors.getDeviceCount(); i++)
  {
    if(sensors.getAddress(tempDeviceAddress, i))
 {
    Serial.print("Found device ");
    Serial.print(i, DEC);
    Serial.print(" with address: ");
    printAddress(tempDeviceAddress);
    Serial.println();
    
    Serial.print("Setting resolution to ");
    Serial.println(TEMPERATURE_PRECISION, DEC);
    
    // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
    sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    
    Serial.print("Resolution actually set to: ");
    Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
    Serial.println();
  }
    //lcd.print(/*dht.readTemperature()*/);
    return sensors.getTempC(tempDeviceAddress);
    break;
  }

}

bool isItHot(float currentTemp) {
  // Megnézi, hogy a mostani hőmérséklet szerint meleg van-e.

  if(currentTemp > 25) {
    return true;
  } else {
    return false;
  }
}

bool isButtonHeld(int pin){
  
  if(digitalRead(pin) == HIGH) {
    if(wasPushed1 == false) {
      wasPushed1 = true;
      return true;
    } else {
      return false;
    }
  } else {
    
    wasPushed1 = false;
    return false;
  }
}
bool isButtonHeld1(int pin){
  
  if(digitalRead(pin) == HIGH) {
    if(wasPushed2 == false) {
      wasPushed2 = true;
      return true;
    } else {
      return false;
    }
  } else {
    
    wasPushed2 = false;
    return false;
  }
}

void manageRGBLED(float currentTemp, int redPin, int bluePin, int greenPin) {
  // Ha hőmérséklet nagyobb mint az első int, a pirosra ír 255-öt, ha kisebb akkor meg a kékre.
  if(isItHot(currentTemp)) {
    // Magas hőmérséklet
    digitalWrite(bluePin, HIGH);
    digitalWrite(greenPin,HIGH);
    digitalWrite(redPin, LOW);
  } else {
    // Alacsony hőmérséklet
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
  }
}
void loop()
{
  float avgTemp = (GetDallasTemp()+ dht.readTemperature()/2);
  
  if(isButtonHeld(7))
  {
    switch (tempeatureHumidity) {
  case 0:
    tempeatureHumidity = 1;
    break;
  case 1:
    tempeatureHumidity = 0;
    break;
  }
  }
  if(isButtonHeld1(8))
  {
    switch (ventilator) {
  case false:
    ventilator = true;
    break;
  case true:
    ventilator = false;
    break;
  }
  }
  
  if(analogRead(5) < 100)
  {
    switch (tempeatureHumidity) {
  case 0:
    lcd.setCursor(1,0);
    lcd.println("Humidity(%):   ");
    lcd.setCursor(1,1);
    lcd.print(dht.readHumidity());
    break;
  case 1:
    lcd.setCursor(1,0);
    lcd.println("Temperature(C):");
    lcd.setCursor(1,1);
    
    lcd.print(avgTemp);
    break;
  } 
  }
  else{ 
    lcd.setCursor(1,0);
    lcd.println("                 ");
    lcd.setCursor(1,1);
    lcd.println("                 ");
    }
  
  if(ventilator && isItHot(avgTemp))
    {analogWrite(6,255);}
    else analogWrite(6,0);
  
  
  manageRGBLED(avgTemp, 9, 11, 10);

}
  













  
