/*
 SMS heather remote control
 
 This sketch, designed for the GSM shield, allows you to remotely control any
 device connected to your Arduino. By sending an SMS, through certain keywords, 
 it's possible, as in this case, to turn on or off a simple heater.
 
 The GSM module, mounted on an Arduino board, processes the message and executes an action.
 Then, it sends a notification message containing the state of the device.
 If the message doesn't contain any valid keyword, the Arduino doesn't execute any 
 operation and the message is deleted from the sim card. 
 The sketch also manages keywords with uppercase or lowercase.
 
 Circuit:
 * GSM shield and an Arduino board
 * SIM card that can send/receive SMS
 * a relay connected to digital pin 12
 
 based on the ReceiveSMS example by Javier Zorzano / TD
 modified by Federico Vanzati & Alberto Cicchi (03/2013)
 
 */

// include the GSM library
#include <GSM.h>

const int heaterControlPin = 12;

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];  

// String to hold the incoming message
String message = "";

// String that will contain the parsed values from the message
String commandID, deviceID;

void setup() 
{
  // set the pin connected to the relay that control the heater as output
  pinMode(heaterControlPin, OUTPUT);

  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while(notConnected)
  {
    if(gsmAccess.begin()==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");

}

void loop() 
{
  char c;

  // If there are any SMS available  
  if (sms.available())
  {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal    
    // Any messages starting with # should be discarded
    // usually anonymous messages starts with #
    if(sms.peek()=='#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while(c = sms.read())
      message += c;

    // cancella message ricevuto 
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  // if there is a new message start to parse the command and the device to control
  if(message != "") {

    // put the incoming characters that compose the message to lower case
    message.toLowerCase();

    int i=0; 

    // deviceID and commandID are separated by a space
    // find it and store the position in two strings
    while(message.charAt(i) != ' ') i++;

    commandID = message.substring(0,i);
    deviceID = message.substring(i+1, message.length());

    // check the device to control
    if(deviceID == "stufetta")
    {
      //check the command to execute
      if(commandID == "accendi") {
        sendFeedbackSMS(senderNumber, deviceID, "ON");  // send the feedback to the sender
        digitalWrite(heaterControlPin, HIGH);
      }
      if(commandID == "spegni") {
        sendFeedbackSMS(senderNumber, deviceID, "OFF"); // send the feedback to the sender
        digitalWrite(heaterControlPin, LOW);
      }
    }

    // clear the message that has just been processed
    message = "";
  }

  delay(1000);
}

// this function, given a phone number and two strings representing the device and 
// its state send a feedback text message to the phone number
void sendFeedbackSMS(char remoteNum[], String devString, String devState) {
  String txtMsg = devString + " " + devState;
  Serial.println(txtMsg);
  Serial.println(remoteNum);

  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS(); 
}

















