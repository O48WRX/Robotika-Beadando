#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Projektfgv.h"

OneWire  ds(A0);              // a A0. kivezetéshez kell kapcsolni a chip data kivezetését 
byte ds_data[9];              //kiolvasott adtok tárolására
float homerseklet;            //ebbe a változóba kerül a kiolvasott hőmérséklet
bool meres=true;              //azért, hogy ne kelljen delay()-t használni, az egyik loop() ciklusban utasítjuk a Dallas chip-et
                              //hogy mérjen, míg a következőben kiolvassuk a mért értéket. Ez jelzi, amikor csak mérni kell
long ido_tmp=millis();   


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

float Homerseklet()
{
  if(millis()>ido_tmp+2000) { //két másodperc után foglalkozunk a hőmérséklet mérés indítással, vagy az eredmény kiolvasással
    if (meres) {              //ha meres=1 akkor arra utasítjuk majd a chip-et, hogy végezzen egy mérést, ha 0 akkor kiolvasunk (else ág)
      if (ds.reset()==1) {    //itt 1-el jelez, ha a reset jelre válaszol egy DS18b20, tehát van a vonalon chip. Ha nincs a vonalon DS18b20, akkor ez 0.
         ds.skip();           //ezzel azt mondjuk a chip-nek, hogy egyedül van a vonalon, nem adunk meg eszköz címet, mindenképpen csinálja amira utasítást kap
         ds.write(0x44,1);    //elinditunk egy mérést a chip-ben
         meres=false;         //ezzel jelezzük, hogy elindítottunk egy mérést, 750msec múlva ki lehet olvasni a mért hőmérsékletet
       }
       else {Serial.println("Nincs DS18b20 a vonalon!");}  //meres változót nem állítjuk false-ra, így várunk amíg lesz chip a vonalon
    }
    else {                    //mivel volt eszköz a vonalon és az előző ciklusban mért is egyet, ki fogjuk olvasni az eredményt
      ds.reset();             //érdekes, de itt már mindenképpen 1 volt a ds.reset() válasza, ha nem volt eszköz a vonalon, akkor is,
                              //így itt már nem is vizsgálom. Viszont mivel itt nem vizsgálom, egy fals mérés lehetséges. 
                              //Ennek eredménye 0.00fok, amiről nem lehet megállapítani, hogy nem valós mért érték.
      ds.skip();              //ezzel azt mondjuk a chip-nek, hogy egyedül van a vonalon, nem adunk meg eszköz címet, mindenképpen csinálja amira utasítást kap
      ds.write(0xBE);         // Chip memóriájának olvasása következik
      for ( byte i=0;i<9;i++) {ds_data[i]=ds.read();}    // 9 bytot olvasunk ki
      //ds_data[3]=0;         //ez egy mesterséges crc hiba előállítás, hogy meglássuk működik-e a crc vizsgálat. Helyes működéshez ki kell kommentezni ezt a sort
      if ( OneWire::crc8(ds_data,8)!=ds_data[8]) {Serial.println("CRC hiba a kiolvasott adatokban!");}   
      else {
        homerseklet=(float) (((ds_data[1]<<8)|ds_data[0])/16.0);          //mert értek visszaadása 
        Serial.print("Hőmérséklet:");Serial.println(homerseklet); 
        meres=true;                                                       //ezzel jelezzük, hogy következő ciklusban mérést kell indítani
      }
    }
    ido_tmp=millis();
    return homerseklet;
}}

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
  float avgTemp = (Homerseklet()+ dht.readTemperature())/2;
  
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
  













  
