/*
 * Based on IRremote Library
 * by Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>
#include <TinkerKit.h>
int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;
 
TKRelay relay(O0);

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if((results.value == 0x80A ) || (results.value == 0xA)){

      OFF();

    } 
    else  if((results.value == 0x80B ) || (results.value == 0xB)){

      ON();

    }else  if((results.value == 0x80C ) || (results.value == 0xC)){

      FLASH(1);

    }
    else  if((results.value == 0x80D ) || (results.value == 0xD)){

      FLASH(3);

    }
    
    irrecv.resume(); // Receive the next value
  }
}
void FLASH(int seconds){
  int i;
  int loops;
  loops = (seconds*1000) / 250;
  for (i = 0; i < loops; i++)  //flashes for n seconds
  {
    relay.on(); 
    delay(80);
    relay.off();
    delay(170);
  }
}
void ON(){
  relay.on();
}

void OFF(){
  relay.off();
}
