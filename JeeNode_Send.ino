//Green School Sensor - load this sketch onto the JeeNode that is sending the data

//JeeLibs library
#include <JeeLib.h>

//Use a watchdog timer for low-power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

//Define RF Node ID and Channel
#define RF_NODEID 1
#define RF_CHANNEL 100

typedef struct {
  int nodeid;
  float soil_1;
  float soil_2;
  float soil_3;
  float temp;
} Payload;
Payload data;

void setup () {
  rf12_initialize(RF_NODEID, RF12_915MHZ, RF_CHANNEL);
  Serial.begin(57600);
}

void loop () {
    while (!rf12_canSend())
    rf12_recvDone();
      data.soil_1 = analogRead(0);
      data.soil_2 = analogRead(1);
      data.soil_3 = analogRead(2);
      data.temp = analogRead(3);
  
  
  // send packet out
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &data, sizeof data);
  rf12_sendWait(2);
  rf12_sleep(RF12_SLEEP);
  
  // go to sleep for approx 5 seconds
  Sleepy::loseSomeTime(5000);
}
