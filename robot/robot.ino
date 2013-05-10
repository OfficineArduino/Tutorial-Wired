/*
 Simple braitenberg vehicle + Obstacle avoiding.
 
 Robot driven by 2 Continuous Rotation Servos.
 Servos controlled by LDRs in front of robot.
 
 The behaviour (light seeking/light avoiding) is determined by which LDR is controlling which servo.
 The analog read values from LDRs are mapped to control the continous rotation servos.
 A tactical switch on pin7 triggers the robot to turn around in case of an obstacle.
 
 The robot can have 3 different behaviours.
 */

#include <Servo.h>

Servo leftServo;
Servo rightServo;

int leftValue = 0;
int rightValue = 0;
int MAXspeed = 0.5; // Value from 0-1

void setup() {              

  // obstacle avoiding switch pin
  pinMode(7, INPUT_PULLUP);   // This line pulls the pin HIGH
  digitalWrite(7, HIGH);

  // LEDeye of robot
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  leftServo.attach(10);
  rightServo.attach(9);
} 

void loop() { 

  // robot hits an obstacle: turns around 180 degree
  if (digitalRead(7)==LOW){
    digitalWrite(6, HIGH);      //blink eye
    delay(100);
    leftServo.write(90);        //stop left servo
    delay(100);
    rightServo.write(90);       //stop left servo
    delay(100);
    digitalWrite(6, LOW);      

    //go back for half a second
    leftServo.write(90 - 10);
    rightServo.write(90 + 25);
    delay(500);

    //turn around right
    leftServo.write(90 + 30);
    rightServo.write(90 + 30);
    delay(700);
  }

  // read LDRs
  leftValue = analogRead(0);
  rightValue = analogRead(1);

  leftValue = map(leftValue, 0, 1023, 0, 89);       // scale it to use it with the servo (value between 0 and 180) 
  rightValue = map(rightValue, 0, 1023, 0, 89);     // scale it to use it with the servo (value between 0 and 180) 

  // First behaviour: calibrating servos for going straight
  // leftServo.write(90 + 18);
  // rightServo.write(90 - 15);  

  // Second behaviour: follow the light
  left.Servo(90 + rightValue);
  right.Servo(90 - leftValue);

  // Third behaviour: avoid the light
  // left.Servo(90 - rightValue);
  // right.Servo(90 + leftValue);

  delay(100);
} 



