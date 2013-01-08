
/*
  Arduino & OpenEnergyMonitor 
  Test & Calibration sketch
  author Mirco Piccin aka pitusso
*/

// Include Emon Library
#include "EmonLib.h"                   

//Calibrations
const int volt = 220;
const float ct_calibration = 29;
const float temp_offset = 1.2;

//Sensor pins
const int tempSensorPin = A0;
const int lightSensorPin = A1;
const int currentSensorPin = A2;

float tempValue = 0;
float Irms = 0;
int lightValue = 0;

// Create an Emon instance
EnergyMonitor emon1;                   

void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("Sensors test starting...");

  //initialize Emon instance
  emon1.current(currentSensorPin, ct_calibration); 
}

void loop() {
  
  //read sensors
  lightValue = analogRead(lightSensorPin);
  tempValue = getCelsius(analogRead(tempSensorPin));
  Irms = emon1.calcIrms(1480);

  //print values
  Serial.print("Temp : ");
  Serial.print(tempValue + temp_offset);
  Serial.print(" ; Light : ");
  Serial.print(lightValue);
  Serial.print(" ; Power : ");
  Serial.println(Irms*volt);

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



