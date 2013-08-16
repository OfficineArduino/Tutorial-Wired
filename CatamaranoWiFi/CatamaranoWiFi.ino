#include <SPI.h>
#include <WiFi.h>
#include <SD.h>
#include <Servo.h>

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   40

const int fanPin = 5;
Servo servo;

char ssid[] = "SSID name";        // write here the name of your network
char pass[] = "secret password";  // write here the WPA password of your network

int status = WL_IDLE_STATUS;

WiFiServer server(80);  // create a server at port 80
File webFile;
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

boolean fanState = false;

void setup()
{
  servo.attach(9);
  pinMode(fanPin, OUTPUT);

  Serial.begin(9600);       // for debugging

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
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  } 

  printWifiStatus();

  // initialize SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");
  // check for index.htm file
  if (!SD.exists("control.htm")) {
    Serial.println("ERROR - Can't find index.htm file!");
    return;  // can't find index file
  }
  Serial.println("SUCCESS - Found index.htm file.");
  pinMode(3, INPUT);        // switch is attached to Arduino pin 3

  server.begin();           // start to listen for clients

}

void loop()
{
  WiFiClient client = server.available();  // try to get client

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        // buffer first part of HTTP request in HTTP_req array (string)
        // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;          // save HTTP request character
          req_index++;
        }
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: keep-alive");
          client.println();
          // Ajax request
          if (StrContains(HTTP_req, "fan")) {
            // read switch state and send appropriate paragraph text
            GetSwitchState(client);
          }
          else if(StrContains(HTTP_req, "dir=")) //strstr(HTTP_req, "dir=") != NULL) 
          {
            client.println("OK");
            char *pch;
            pch = strtok(HTTP_req, " =");
            while (strcmp(pch, "/dir")) {
              pch = strtok(NULL, " =");
            }
            pch = strtok(NULL, " =");
            Serial.print("dir val: ");
            int dir = atoi(pch);
            Serial.println(dir);
            analogWrite(fanPin,   dir);
            servo.write(dir);
          }
          else {  // web page request
            // send web page
            webFile = SD.open("control.htm");        // open web page file
            if (webFile) {
              while(webFile.available()) {
                client.write(webFile.read()); // send web page to client
              }
              webFile.close();
            }
          }
          // display received HTTP request on serial port
          //Serial.println(HTTP_req);
          // reset buffer index and all buffer elements to 0
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}

// send the state of the switch to the web browser
void GetSwitchState(WiFiClient cl)
{
  fanState = !fanState;

  digitalWrite(fanPin, fanState);

  if (fanState) {
    cl.println("FAN is ON");
  }
  else {
    cl.println("FAN is OFF");
  }
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return index;
      }
    }
    else {
      found = 0;
    }
    index++;
  }

  return 0;
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









