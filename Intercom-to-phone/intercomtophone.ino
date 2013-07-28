#include <GSM.h>
#include <Servo.h>

// PIN Number
String PINNUMBER =  ""
const int SERVOPIN = 9
const int THRESHOLD = 100 //Adjust your own threshold
const int PIEZOPIN = A0
const int STARTPOS = 120
const int ENDPOS = 160
// initialize the library instance
GSM gsmAccess; // include a 'true' parameter for debug enabled
GSMVoiceCall vcs;
Servo replyServo;
String remoteNumber = "0123456789";  // the number you will call
char charbuffer[20];

void setup()
{
 //initialize the Servo
 replyServo.attach(SERVOPIN);
 // connection state
 boolean notConnected = true;
   
 // Start GSM shield
 // If your SIM has PIN, pass it as a parameter of begin() in quotes
 while(notConnected)
 {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
     notConnected = false;
    else
    {
     delay(1000);
    }
 }
  //prepare the string containing the phone number
 remoteNumber.toCharArray(charbuffer, 20);
}

void loop()
{

      //someone is ringing?
      if(analogRead(PIEZOPIN) > THRESHOLD) {

       
       // Check if the receiving end has picked up the call
       if(vcs.voiceCall(charbuffer))
       {
         //activate the intercom
         replyServo.write(STARTPOS);
         
         // Wait for the calling end
         while(vcs.getvoiceCallStatus()==TALKING);          
         //deactivate the intercom
         replyServo.write(ENDPOS);
         // And hang up
         vcs.hangCall();          
         
       }
       
      }
}
