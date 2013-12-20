/*
  Cloud Scale - A weight scale connected to Internet
  with Arduino Yún & Temboo

  You can hack a scale and read the weight with Arduino, 
  adding buttons you can associate a user to each button,
  then the Yún will add the ability to configure the user names 
  through a simple we interface who resides inside the onboard server
  and log the weight trend of each user inside a google spreadsheet.

  Upload the sketch using the networking port to load the web page
  that sets the buttons. 
  You can reach the page at the url:
  http://myYun.local/sd/conf.htm

  created by Federico Vanzati
  November 2013
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
			   // fill the TembooAccount.h file with:
                           //  * Temboo credentials
			   //  * Google credentials

#define MAX_LEN 10

int scaleZero;

const int b1_pin = 3,
           b2_pin = 6,
           b3_pin = 8,
           b4_pin = 12;

boolean prevBtn1State = HIGH,
        prevBtn2State = HIGH,
        prevBtn3State = HIGH,
        prevBtn4State = HIGH;


void setup() {
  // Bridge startup
  Bridge.begin();

  analogRead(A0);
  scaleZero = analogRead(A0);

  pinMode(b1_pin, INPUT_PULLUP);
  pinMode(b2_pin, INPUT_PULLUP);
  pinMode(b3_pin, INPUT_PULLUP);
  pinMode(b4_pin, INPUT_PULLUP);

  Bridge.put(F("button1"), F("Pippo"));
  Bridge.put(F("button2"), F("Pluto"));
  Bridge.put(F("button3"), F("Topolino"));
  Bridge.put(F("button4"), F("Mario"));

  Serial.begin(115200);
  while (!Serial);

  Serial.println(scaleZero);
}

void loop() {
  char userName[MAX_LEN];
  int whichBtn = buttonPressed();

  if (whichBtn != 0) {
    String key("button");
    key += whichBtn;
    Bridge.get(key.c_str(), userName, MAX_LEN);

    // Print on the Serial Monitor
    Serial.print(F("button pressed: "));
    Serial.println(whichBtn);

    long int sensorReading = 0;
    for (int i = 0; i < 200; i++)
      sensorReading += analogRead(A0);

    sensorReading = (sensorReading / 200 - scaleZero) * 2;

    String data = getTimeStamp();
    data += ',';
    data += userName;
    data += ',';
    data += sensorReading;

    Serial.println(data);

    //updateDisplay(userName, sensorReading);

    // Update the Spreadsheet
    updateSpreadSheet(data);

  }
  delay(50);
}

// The buttonPressed function read 4 buttons and return which button has been pressed
// return 0 if no button is pressed
int buttonPressed() {
  int btnCode = 0;
  // Read all the buttons
  boolean b1 = digitalRead(b1_pin);
  boolean b2 = digitalRead(b2_pin);
  boolean b3 = digitalRead(b3_pin);
  boolean b4 = digitalRead(b4_pin);

  if (b1 == LOW && b1 != prevBtn1State)
    btnCode = 1;
  if (b2 == LOW && b2 != prevBtn2State)
    btnCode = 2;
  if (b3 == LOW && b3 != prevBtn3State)
    btnCode = 3;
  if (b4 == LOW && b4 != prevBtn4State)
    btnCode = 4;

  prevBtn1State = b1;
  prevBtn2State = b2;
  prevBtn3State = b3;
  prevBtn4State = b4;

  return btnCode;
}


// The getTimeStamp function return a string with the time stamp
// in the MM/DD/YYYY-hh:mm:ss format
String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin(F("date"));
  time.addParameter(F("+%D-%T"));  // parameters: D for the complete date mm/dd/yy
  // 		  T for the time hh:mm:ss
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n')
      result += c;
  }

  return result;
}

// The updateSpreadSheet function publish to the associate spreadsheet a row of data
// param @data  string containing the row of data to publish on the spreadsheet
//		each column is comma separated
void updateSpreadSheet(String data) {
  TembooChoreo AppendRowChoreo;

  // invoke the Temboo client
  AppendRowChoreo.begin();

  // set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

  // set choreo inputs
  AppendRowChoreo.addInput(F("Username"), GOOGLE_USER);
  AppendRowChoreo.addInput(F("Password"), GOOGLE_PWD);
  AppendRowChoreo.addInput(F("RowData"), data);
  AppendRowChoreo.addInput(F("SpreadsheetTitle"), F("WeightYunScale"));

  // identify choreo to run
  AppendRowChoreo.setChoreo(F("/Library/Google/Spreadsheets/AppendRow"));

  // run choreo; when results are available, print them to serial
  AppendRowChoreo.run();

  while (AppendRowChoreo.available()) {
    char c = AppendRowChoreo.read();
    Serial.print(c);
  }
  AppendRowChoreo.close();
}


