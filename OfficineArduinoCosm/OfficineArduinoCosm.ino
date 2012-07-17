/*
  Pachube sensor client with Strings
 
 This sketch connects an analog sensor to Cosm (http://www.cosm.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * Thermistor attached to analog in 0
 * LDR sensor attached to analog in 1
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 Based on the CosmClientString created:
 by Tom Igoe with input from Usman Haque and Joe Saavedra
 customized by Federico Vanzati
 
 http://arduino.cc/en/Tutorial/PachubeClientString
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet.h>

                                                                     
#define APIKEY         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" // replace your Cosm api key here
#define FEEDID         67409 // replace your feed ID
#define USERAGENT      "Officine Arduino Temperature and Light" // user agent is the project name

const static int Beta = 3950;        // Beta parameter
const static float Kelvin = 273.15;  // 0°C = 273.15 K
const static int Rb = 10000;         // 10 kOhm
const static float Ginf = 120.6685;  // Ginf = 1/Rinf
//The formula to compute the Rinf is:
// Rinf = R0*e^(-Beta/T0) = 4700*e^(-3950/298.15)


// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,100);

// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(216,52,233,121);      // numeric IP for api.cosm.com
char server[] = "api.cosm.com";   // name address for Cosm API

unsigned long lastConnectionTime = 0;  // last time you connected to the server, in milliseconds
boolean lastConnected = false;  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to Pachube.com

void setup() {
  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  } 
  else {
    Serial.print("My IP address is: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }
    Serial.println();
  }
}

void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);
  float Rthermistor = Rb * (1023.0 / sensorReading - 1);
  float temperatureC = Beta / (log( Rthermistor * Ginf )) ;
  temperatureC -=  Kelvin;

  int decimal = (int)temperatureC;
  int fraction = (temperatureC - decimal) * 10;

  String dataString = "Temperatura_Ufficio,";
  dataString += decimal;
  dataString += '.';
  dataString += fraction;

  //Reading the LDR and transform in percentage of light
  int ldrReading = map(analogRead(A1), 0, 1023, 0, 100);
  
  dataString += "\nIlluminazione,";
  dataString += ldrReading;

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
//    Serial.print(decimal);
//    Serial.print(".");
//    Serial.println(fraction);
//    Serial.println();
    Serial.println(dataString);
    Serial.println();
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.cosm.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.println(thisData);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
    lastConnectionTime = millis();
}







