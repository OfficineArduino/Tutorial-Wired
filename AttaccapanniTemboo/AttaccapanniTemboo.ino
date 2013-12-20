/*
  Meteorologist Hanger
  with Arduino Yún and Temboo

  This sketch uses the Yahoo Wather Temboo Choreo's to obtain the
  weather forecast and turn on 5 LEDs, placed on an hanger,
  corresponding to 5 selected weather conditions:
  good, cloudy, storm and snow.
  This will help you to chose the sun glasses, the coat or the umbrella
  before you go out.

  Based on the YahooWeather example

  created by Federico Vanzati
  October 2013
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below
#include "WeatherCode.h"

String libraryPath = "/rss/channel/item/yweather:";

void setup() {

  pinMode(GOOD, OUTPUT);
  pinMode(CLOUDY, OUTPUT);
  pinMode(STORM, OUTPUT);
  pinMode(SNOW, OUTPUT);

  Serial.begin(9600);

  // For debugging, wait until a serial console is connected.
  delay(4000);
  while (!Serial);
  Bridge.begin();
}
void loop()
{
  TembooChoreo GetWeatherByAddressChoreo;

  // invoke the Temboo client
  GetWeatherByAddressChoreo.begin();

  // set Temboo account credentials
  GetWeatherByAddressChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetWeatherByAddressChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetWeatherByAddressChoreo.setAppKey(TEMBOO_APP_KEY);

  // set choreo inputs
  GetWeatherByAddressChoreo.addInput("Units", "c");
  GetWeatherByAddressChoreo.addInput("Address", "Torino");

  // identify choreo to run
  GetWeatherByAddressChoreo.setChoreo("/Library/Yahoo/Weather/GetWeatherByAddress");


  //GetWeatherByAddressChoreo.addOutputFilter("temperature", libraryPath+"condition/@temp", "Response");

  GetWeatherByAddressChoreo.addOutputFilter("condition", libraryPath + "condition/@code", "Response");

  // run choreo; when results are available, print them to serial
  GetWeatherByAddressChoreo.run();

  String output[5];
  int i = 0;

  while (GetWeatherByAddressChoreo.available()) {
    /*
    byte c = GetWeatherByAddressChoreo.read();
    Serial.println(c);
    */
    char c = GetWeatherByAddressChoreo.read();
    if (c == 30 || c == 31) continue;  // skip record and unit separator characters
    if (c != '\n') {
      output[i] += c;
    } else {
      i++;
    }
  }
  Serial.println(output[0]);
  Serial.println(codeToCondition[output[1].toInt()]);
  Serial.println(output[2]);
  Serial.println(output[3]);
  Serial.println(output[4]);

  digitalWrite(GOOD, LOW);
  digitalWrite(CLOUDY, LOW);
  digitalWrite(STORM, LOW);
  digitalWrite(SNOW, LOW);

  digitalWrite(codeToCondition[output[1].toInt()], HIGH);


  GetWeatherByAddressChoreo.close();

  Serial.println("Waiting...");
  delay(10 * 60000L); // wait 10 minutes between GetWeatherByAddress calls
}
