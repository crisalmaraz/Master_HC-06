#include <SoftwareSerial.h>
#include "max6675.h"
#include <Wire.h>
#include <DallasTemperature.h>
#include "SparkFunBME280.h"
#include <ArduinoJson.h>


SoftwareSerial BT1(3, 2); // RX | TX
MAX6675 thermocouple(13, 5, 12);//lector de temperatura con rosca

OneWire oneWire(2);  //ds18b20
DallasTemperature sensors(&oneWire);

DeviceAddress P1 = { 0x28, 0xFF, 0xB0, 0x87, 0x8C, 0x16, 0x03, 0x25 }; //P1
DeviceAddress P2 = { 0x28, 0xFF, 0x61, 0x6C, 0xA0, 0x16, 0x03, 0x44 }; //P2

float lec=0;
BME280 bme;

const size_t capacity = JSON_OBJECT_SIZE(8);
DynamicJsonDocument doc(capacity);

double temp=0.0;

void setup(){ 
     pinMode(A3,INPUT);
     
      bme.settings.commInterface = I2C_MODE;
      bme.settings.I2CAddress = 0x76;
      bme.settings.runMode = 3; //Normal mode
      bme.settings.tStandby = 0;
      bme.settings.filter = 0;//0,1,2,3,4
      bme.settings.tempOverSample = 1;
      bme.settings.pressOverSample = 1;
      bme.settings.humidOverSample = 1;
  
     delay (500);
     Serial.begin(9600);
     Serial.println("Iniciando Transmisor");
     Serial.println("Iniciando comunicación con Bluetooth HC-06");

     BT1.begin(9600); 
     
     sensors.begin();//para los 2 ds18b20
     sensors.setResolution(P1, 9);//a menor resolucion mas rapido de 9-12
     sensors.setResolution(P2, 9);

     delay(500);

      if(!bme.begin()){
        Serial.println("Error bme");
      }
   }

void loop(){      
    delay(2000);
    lec=thermocouple.readCelsius();
    doc["TempTermocople"] = lec;
    doc["TempDs18b20_1"] = lec;
    doc["TempDs18b20_2"] = lec;
    //sensors.requestTemperatures(); 
    //doc["TempDs18b20_1"] = sensors.getTempCByIndex(1);//para primer ds18b20
    //doc["TempDs18b20_2"] = sensors.getTempCByIndex(0);//para segundo ds18b20
    doc["TempBme280"] = bme.readTempC();
    doc["HumedadBme280"] = bme.readFloatHumidity();
    doc["PresionBme280"] = bme.readFloatPressure();
    doc["Altitud_m"] = bme.readFloatAltitudeMeters();
    doc["HumedadResistiva"] = map(analogRead(A3),0,1023,100,0);
    
    serializeJson(doc, BT1);
    serializeJson(doc, Serial);
    Serial.println();
    
   }
