// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;
int sensor = A1;

void setup () {
    Serial.begin(9600);
    Wire.begin();
    RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop () {
    DateTime now = RTC.now();
    
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    
    int mins = now.minute();
    if (mins < 10)
      Serial.print('0');
      
    Serial.print(mins);
    Serial.print(':');
    
    int secs = now.second();
    if (secs < 10)
      Serial.print('0');
      
    Serial.print(secs);
    Serial.println();
    
    int sensorValue = analogRead(sensor);
    
    float voltage = sensorValue * (5.0 / 1023.0);
    
    Serial.print("Sensor Voltage = ");
    Serial.print(voltage);
    Serial.println("V");
    Serial.println();
    
    delay(2000);
}
