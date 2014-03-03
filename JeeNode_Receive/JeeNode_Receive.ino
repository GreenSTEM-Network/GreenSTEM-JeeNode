
#include <JeeLib.h>

//Define RF Node ID and Channel
#define RF_NODEID 2
#define RF_CHANNEL 100
#define RF_FREQUENCY RF12_433MHZ

typedef struct {
  int nodeid;
  float soil_1;
  float soil_2;
  float soil_3;
  float temp;
  int vccRead;
} Payload;

Payload data;

#define SERIAL_BAUD 57600

void setup() {  
    rf12_initialize(RF_NODEID, RF_FREQUENCY, RF_CHANNEL);
    Serial.begin(57600);
}

void loop() {

  if (rf12_recvDone() && rf12_crc == 0 &&
              rf12_len == sizeof (data)) {
    data = *(Payload*) rf12_data;

    Serial.print(data.soil_1);
    Serial.print(',');
    Serial.print(data.soil_2);
    Serial.print(',');
    Serial.print(data.soil_3);
    Serial.print(',');
    Serial.print(data.temp);  
    Serial.print(",");
    Serial.println(data.vccRead);
    
    if (RF12_WANTS_ACK) {
      rf12_sendStart(RF12_ACK_REPLY, 0 ,0);
    }
  }
}
