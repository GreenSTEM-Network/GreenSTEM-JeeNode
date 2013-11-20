//Green School Sensor - load this sketch onto the JeeNode that is sending the data
//This is a test sketch for use with one soil moisture sensor

//JeeLibs library
#include <JeeLib.h>\

//Use a watchdog timer for low-power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

//Define RF Node ID and Channel
#define RF_NODEID 1
#define RF_CHANNEL 004 //Channel# corresponds to kit ID#

typedef struct {
  int nodeid;
  float soil_1;
} Payload;
Payload data;

void setup () {
  rf12_initialize(RF_NODEID, RF12_915MHZ, RF_CHANNEL);
  Serial.begin(57600);
  pinMode(5, OUTPUT);  //Set digital pin 5 (pin D, port 2 on JeeNode) as a power output
}

void loop () {
    //while (!rf12_canSend())
    //  rf12_recvDone();
      digitalWrite(5, HIGH); // turn on power to sensors
      delay(400);
      data.soil_1 = analogRead(0);
  
  // send packet out
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &data, sizeof data);
  rf12_sendWait(2);
  rf12_recvDone();
  rf12_sleep(RF12_SLEEP);
  
  // turn off power to sensors
  digitalWrite(5,LOW);
  
  // go to sleep for approx 30 min (comment this out for testing purposes)
  //for (int k=0; k <30; k++) {
  //  Sleepy::loseSomeTime(60337);
    
  // go to sleep for 10 secs (uncomment this for testing purposes)
  for (int k=0; k <1; k++) {
    Sleepy::loseSomeTime(10000);
  }
}
