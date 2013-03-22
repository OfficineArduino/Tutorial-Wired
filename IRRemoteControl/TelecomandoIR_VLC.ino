/* VLC IR REMOTE CONTROLLER
 *
 * Control your computer while using VLC 
 * with any IR remote controller you want.
 *
 * Created by
 * Angelo Scialabba
 * Arturo Guadalupi
 *
 * Based on IRremote library by Ken Shirriff  
 */

#include <IRremote.h>
int RECV_PIN = 11;  //IR receiver connected on pin 11

//Change these values to match data sent by your remote control

/*-----COMMANDS------*/
const long play = 0x800F046E;  
const long volumeUp= 0x800F0410;
const long volumeDown= 0x800F8411;
const long forward= 0x800F8414;
const long backward= 0x800F0415;
const long mute= 0x800F040E;
/*-----END COMMANDS------*/

IRrecv irrecv(RECV_PIN);  //initialize IR library on RECV_PIN
decode_results results;   //received data will be stored here

void setup()
{
  irrecv.enableIRIn();  // Start the receiver
  Keyboard.begin();  //Start arduino as keyboard
}

void loop() {
  if (irrecv.decode(&results)) {
    switch(results.value){  //fetch received data
    case play:
      Keyboard.press(' ');
      break;
    case volumeUp:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_UP_ARROW);
      break;
    case volumeDown:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_DOWN_ARROW);
      break;
    case forward:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_RIGHT_ARROW);
      break;
    case backward:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ARROW);
      break;
    case mute:
      Keyboard.press('m');
      break;
    }
    delay(100);
    Keyboard.releaseAll();

    irrecv.resume(); // Receive the next value
  }
}
