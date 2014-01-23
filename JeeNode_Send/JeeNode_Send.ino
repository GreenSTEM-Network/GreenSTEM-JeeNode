//Green School Sensor - load this sketch onto the JeeNode that is sending the data

//JeeLibs library
#include <JeeLib.h>

//Use a watchdog timer for low-power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

//Define RF Node ID and Channel
#define RF_NODEID 1
#define RF_CHANNEL 100
#define RF_FREQUENCY RF12_433MHZ
#define TIME_BETWEEN_READS 5000 // in milliseconds

//Define physical analog pins to read from
#define SOIL1_PIN 0
#define SOIL2_PIN 1
#define SOIL3_PIN 2
#define TEMPERATURE_PIN 3

typedef struct {
  int nodeid;
  float soil_1;
  float soil_2;
  float soil_3;
  float temp;
} Payload;

Payload data;

void setup () {
  rf12_initialize(RF_NODEID, RF_FREQUENCY, RF_CHANNEL);
  Serial.begin(57600);
}

void loop () {

  //Wait until JeeNode is ready to send data
  while (!rf12_canSend()) {    
    rf12_recvDone();
  }
  
  //Read data from sensors
  data.soil_1 = analogRead(SOIL1_PIN);
  data.soil_2 = analogRead(SOIL2_PIN);
  data.soil_3 = analogRead(SOIL3_PIN);
  data.temp = analogRead(TEMPERATURE_PIN);
  
  // send packet out
  send_data();
  
  // go to sleep for approx 5 seconds
  Sleepy::loseSomeTime(TIME_BETWEEN_READS);
}

void send_data() {
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &data, sizeof data);
  rf12_sendWait(2);
  rf12_sleep(RF12_SLEEP);
}
