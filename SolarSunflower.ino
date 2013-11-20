
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <util.h>
#include <Dns.h>
#include <Dhcp.h>
#include <EthernetUdp.h>
#include <stdlib.h>
#include <SPI.h>
#include <aJSON.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 RTC;
char ssid[] = "";     //  your network SSID (name) 
char pass[] = "";    // your network password

int status = WL_IDLE_STATUS;     // the Wifi radio's status

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "10.0.1.14";
//IPAddress server(172,20,10,6);

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 5000;  // delay between updates, in milliseconds

void setup() {
  // start serial port:
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    Serial.begin(9600);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
  } 
  // you're connected now, so print out the status:
  printWifiStatus();
}

// this method makes a HTTP connection to the server:
void httpPOSTRequest(String val4, String val5, String val6) {
  printWifiStatus();
  // if there's a successful connection:
  if (client.connect(server, 3000)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    String PostData = "{\"type\":\"soilmoisture\", \"data\":{\"collection_point_id\":1,\"collection_time\":\"2013-03-19 08:58AM\",\"deptha\":" + val4 + ",\"depthb\":" + val5 + ",\"depthc\":" + val6 + "}}";

    client.println("POST /dc HTTP/1.1");
    client.println("Host: 172.20.10.6:3000");
    client.println("Connection: keep-alive");
    //the following two lines are the fix andy came up with at code for philly on 4/2/2013 - KBC
    client.print("Content-Length: ");
    client.println( PostData.length()); 
    client.println("Content-Type: application/json");
    client.println();
    client.print(PostData);
    client.flush();

    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // if there's a successful connection:
  if (client.connect(server, 3000)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

float readSensor(int readAddress, int numberOfSensorReads) {
  float voltage = 0.0;
  for (int iterations = 1; iterations <= numberOfSensorReads; iterations++) { 
    voltage = voltage + (5.00 / 1023.00) * readAddress;
  }
  float sensorValue = voltage / numberOfSensorReads;
  sensorValue = (sensorValue - 1.6)/.0167;  //convert voltage to Volumetric Water Content for Vegetronix VG400 E
  return sensorValue;
}

boolean clockCheck() {
  if (! RTC.isrunning()) {
    //Notifies the user via serial monitor that the real-time clock is not functional
    Serial.println("RTC is NOT running!");
    //Sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
    return false;
  }
  else{
    return true;
  }
}

String timestamp(){
  String timestamp = "";

  if (clockCheck()){
    DateTime now = RTC.now();
     
    //all these Serial.prints are to test generation of timestamp - can be deleted once it's working 
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
    char Buffer[20] = "";
    //char* date_value = "02/23/2013";
    //String sensorOne = dtostrf(sensorOneValue, 2, 1, Buffer);
    String month = dtostrf(now.month(), 2, 1, Buffer);
    String day = dtostrf(now.day(), 2, 1, Buffer);
    String year = dtostrf(now.year(), 2, 1, Buffer);
    
    String hour = dtostrf(now.hour(), 2, 1, Buffer);
    String minute = dtostrf(now.minute(), 2, 1, Buffer);
    String second = dtostrf(now.second(), 2, 1, Buffer);

    timestamp = month + '/' + day + '/' + year + ' ' + hour + ':' + minute  + ':' + second;
  }
  else {
    Serial.print('Clock uninitialized!');
    timestamp = "none";
  }
  return timestamp;
}

float readTemperature (int readAddress, int numberOfSensorReads){
  float temp = 1023/readAddress - 1;
  temp = 10000 / temp; //resistor value 10k div by value
  temp = temp / 10000;
  temp = log(temp);                  // ln(R/Ro)
  temp /= 3950;                   // 1/B * ln(R/Ro)
  temp += 1.0 / (25 + 273.15); // + (1/To)
  temp = 1.0 / temp;                 // Invert
  temp -= 273.15;                         // convert to C
  temp = (temp*9.0)/5.0 + 32.0;     // convert to F
  return temp;
}

void loop() {
  //checks to see that the real-time clock is functioning
  clockCheck();

  //resets all data regarding client connection
  client.flush();

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
  
  int numberOfSensorReads = 5;
  int readValue = 0;

  readValue = analogRead(A1);
  float sensorOneValue = readSensor(readValue, numberOfSensorReads);

  readValue = analogRead(A2);
  float sensorTwoValue = readSensor(readValue, numberOfSensorReads);
  
  readValue = analogRead(A3);
  float sensorThreeValue = readSensor(readValue, numberOfSensorReads);

  int sensor_id = 1;
  char* date_value = "02/23/2013";
  char* time_value = "16:04:00";
  char Buffer[20] = "";
  String sensorOne = dtostrf(sensorOneValue, 2, 1, Buffer);
  String sensorTwo = dtostrf(sensorTwoValue, 2, 1, Buffer);
  String sensorThree = dtostrf(sensorThreeValue, 2, 1, Buffer);
  
// if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpPOSTRequest(sensorOne, sensorTwo, sensorThree);
    //httpRequest();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
