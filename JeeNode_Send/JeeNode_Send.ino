//Green School Sensor - load this sketch onto the JeeNode that is sending the data

//JeeLibs library
#include <JeeLib.h>

//Define RF Node ID and Channel
#define RF_NODEID 1
#define RF_CHANNEL 100
#define RF_FREQUENCY RF12_433MHZ
#define TIME_BETWEEN_READS 10000 // in milliseconds

//Use a watchdog timer for low-power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

typedef struct {
  int nodeid;
  float soil_1;
  float soil_2;
  float soil_3;
  float temp;
  int vccRead;
} Payload;
Payload data;

//Voltage reading in mV using 1.1V bandgap reference
static int vccRead (byte us =250) {
  analogRead(6); // set up "almost" the proper ADC readout
  bitSet(ADMUX, 3); // then fix it to switch to channel 14
  delayMicroseconds(us); // delay substantially improves accuracy
  bitSet(ADCSRA, ADSC);
        while (bit_is_set(ADCSRA, ADSC))
         ;
  word x = ADC;
  return x ? (1100L * 1023) / x : -1;
}

void setup () {
  rf12_initialize(RF_NODEID, RF_FREQUENCY, RF_CHANNEL);
  Serial.begin(57600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop () {
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); // bring power to high
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  delay(400);
  
  float soil1_sum = 0;
  float soil2_sum = 0;
  float soil3_sum = 0;
  float temp_sum = 0;
  
  for (int i=0; i<5; i++){
     data.soil_1 = analogRead(0);
     soil1_sum += data.soil_1;
     delay(10);                                                                                                                      
     data.soil_2 = analogRead(1);
     soil2_sum += data.soil_2;
     delay(10);  
     data.soil_3 = analogRead(2);
     soil3_sum += data.soil_3;
     delay(10);    
     data.temp = analogRead(3);
     temp_sum += data.temp;
     delay(500);
  }
  
  data.soil_1 = soil1_sum / 5;
  data.soil_2 = soil2_sum / 5;
  data.soil_3 = soil3_sum / 5;
  data.temp = temp_sum / 5;
  data.vccRead = vccRead();
  
  //temp conversion: Fahrenheit = (((sensorvalue*3.3/1024)/.02)*1.8) + 32
  
  // send packet out
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &data, sizeof data);
  rf12_sendWait(2);
  rf12_recvDone();
  rf12_sleep(RF12_SLEEP);
  
  // put power on low
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  
  // go to sleep for approx 30 min
  //for (int k=0; k <30; k++) {
   //Sleepy::loseSomeTime(60337);
    
  // go to sleep for 10 secs  
  for (int k=0; k <1; k++) {
    Sleepy::loseSomeTime(TIME_BETWEEN_READS);
  }
}
