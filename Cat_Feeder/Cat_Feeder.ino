#include <Servo.h>

Servo myServo;            // viene creato un oggetto "servomotore"
int sensorValue, first = 0;

void setup() {
  myServo.attach(9);      // Il servomotore è controllato dal pin 9
  Serial.begin(9600);     // La porta seriale è abilitata per eventuale debug
  while(digitalRead(8)) { // Il barattolo è inizialmente chiuso fino 
    myServo.write(45);    // alla ricezione della prima chiamata
    first = 1;
  }
}

void loop() {
  if(first){
    sensorValue = digitalRead(8);
    if(sensorValue)       // Se il sensore ldr rileva della luce
      myServo.write(45);  // il servomotore chiude il contenitore
    else                  // altrimenti
      myServo.write(120); // il servomotore apre il contenitore
  }
}
