/*
  Arduino & OpenEnergyMonitor 
  
  This sketch connects to an emoncms server and makes a request using
  Arduino Ethernet shield (or other based on Wiznet W5100) or an 
  Arduino Wifi Shield
  
  author Mirco Piccin aka pitusso
  
  based on 
  http://arduino.cc/en/Tutorial/WebClientRepeating
*/


//if using a W5100 based Ethernet shield, comment out the following line; 
//leave untouched if using Arduino Wifi Shield
#define WIFI

#include <SPI.h>

#ifdef WIFI
  #include <WiFi.h>
  #include <WiFiClient.h>
#else
  #include <Ethernet.h>
#endif

// Include Emon Library
#include "EmonLib.h"                   

//network configuration, WIRED or WIFI
#ifdef WIFI
  //if using WIFI 
  char ssid[] = "ssid"; //  your network SSID (name) 
  char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)

  int status = WL_IDLE_STATUS;
  int keyIndex = 0;            // your network key Index number (needed only for WEP)
  
  WiFiClient client;
#else
  //if using WIRED
  byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x69, 0xD5};
  
  // fill in an available IP address on your network here,
  // for auto configuration:
  IPAddress ip(192, 168, 1, 2);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gw_dns(192, 168, 1, 254);
  
  EthernetClient client;
#endif

//Calibrations
const int volt = 220;
const float ct_calibration = 29;
const float temp_offset = 0;

// Sensor pins
const int tempSensorPin = A0;
const int lightSensorPin = A1;
const int currentSensorPin = A2;

float tempValue = 0;
float Irms = 0;
int lightValue = 0;

// Create an Emon instance
EnergyMonitor emon1;                   

//Emoncms configurations
char server[] = "emoncms.org";     // name address for emoncms.org
//IPAddress server(213, 138, 101, 177);  // numeric IP for emoncms.org (no DNS)

String apikey = "abababcdcdcdefefef001122334567890";  //api key
String node = "1"; //if blank, not used

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

void setup() {
  // start serial port:
  Serial.begin(9600);

  // Display a welcome message
  Serial.println("Emoncms client starting...");

  emon1.current(currentSensorPin, ct_calibration);

#ifdef WIFI
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. 
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
#else
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("Failed to get an IP address using DHCP, forcing manually");
    Ethernet.begin(mac, ip, gw_dns, gw_dns);
  }
#endif  

  printStatus();
}

void loop() {
  
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
    Serial.println("Disconnecting...");
    client.stop();
  }
  
  // if you're not connected, and at least <postingInterval> milliseconds have
  // passed sinceyour last connection, then connect again and
  // send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
   
    //read sensors
    lightValue = analogRead(lightSensorPin);
    tempValue = getCelsius(analogRead(tempSensorPin));
    Irms = emon1.calcIrms(1480);
    
    //Print values (debug)
    Serial.println();
    Serial.print("Temp : ");
    Serial.print(tempValue);
    Serial.print(" ; Light : ");
    Serial.print(lightValue);
    Serial.print(" ; Power : ");
    Serial.println(Irms*volt);
      
    //send values
    sendData();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP GET request:
    client.print("GET /api/post?apikey=");
    client.print(apikey);
    if (node != "") {
      client.print("&node=");
      client.print(node);
    }
    client.print("&json={temp");
    client.print(":");
    client.print(tempValue + temp_offset);    
    client.print(",light:");
    client.print(lightValue);
    client.print(",power:");
    client.print(Irms*volt);   
    client.println("} HTTP/1.1");
    client.println("Host:emoncms.org");
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting...");
    client.stop();
  }
}


void printStatus() {
  #ifdef WIFI
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
    Serial.print(" dBm");
  #else
    // print your local IP address:
    Serial.print("IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }  
  #endif
  Serial.println();
}

float getCelsius(int sensorValue) {
/*
  created by Federico Vanzati for TinkerKit Thermistor Library
*/
  const static float ADCres = 1023.0;
  const static int Beta = 3950;			// Beta parameter
  const static float Kelvin = 273.15;	// 0°C = 273.15 K
  const static int Rb = 10000;			// 10 kOhm
  const static float Ginf = 120.6685;	// Ginf = 1/Rinf 
  
  float Rthermistor = Rb * (ADCres / sensorValue - 1);
  float _temperatureC = Beta / (log( Rthermistor * Ginf )) ;
  return _temperatureC - Kelvin;
}


