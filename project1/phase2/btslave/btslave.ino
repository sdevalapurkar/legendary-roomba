#include <Servo.h>

String readStringg; //main captured String 
String joyRButtonState; //data String
String joyRX;
String joyRY;
bool toggle = true;
int laserPin = 2;
int ind1; // , locations
int ind2;
int ind3;
int initial_positionX = 100;
int initial_positionY = 80;
Servo servoX;
Servo servoY;
int servoY_pin = 8;
int servoX_pin = 9;
 
void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  servoY.attach(servoY_pin);
  servoX.attach(servoX_pin);
  servoY.write(initial_positionY);
  servoX.write(initial_positionX);
}

void loop() {
  if (Serial1.available())  {
    char c = Serial1.read();  //gets one byte from serial buffer
    
    if (c == '*') {
      ind1 = readStringg.indexOf(',');  //finds location of first ,
      joyRButtonState = readStringg.substring(0, ind1);   //captures first data String
      ind2 = readStringg.indexOf(',', ind1+1 );   //finds location of second ,
      joyRX = readStringg.substring(ind1+1, ind2);   //captures second data String
      ind3 = readStringg.indexOf(',', ind2+1 );
      joyRY = readStringg.substring(ind2+1, ind3);

      if (joyRButtonState.toInt() == 0) {
        delay(50);  //delay for debounce
    
        if(toggle) {
          Serial.print("hey");
          digitalWrite(laserPin, HIGH);   // set the LED on
          toggle = !toggle;
        } else {
          digitalWrite(laserPin, LOW);    // set the LED off
          toggle = !toggle;
        }
      }

      if (joyRX.toInt() < 300) {
        Serial.print("yoooo");
        if (initial_positionX < 10) { 
        } else { 
          Serial.print("guiuuuuuy");
          initial_positionX = initial_positionX - 1; 
          servoX.write (initial_positionX);
          delay(50); 
        } 
      } 
  
      if (joyRX.toInt() > 700) {
        Serial.print("yoooo22222");
        if (initial_positionX > 180) {  
        } else {
          initial_positionX = initial_positionX + 1;
          servoX.write (initial_positionX);
          delay(50);
        }
      }
  
      if (joyRY.toInt() < 300){
        if (initial_positionY < 10) { 
        } else { 
          initial_positionY = initial_positionY - 1; 
          servoY.write (initial_positionY);
          delay(50); 
        } 
      } 
  
      if (joyRY.toInt() > 700){
        if (initial_positionY > 180){  
        } else{
          initial_positionY = initial_positionY + 1;
          servoY.write (initial_positionY);
          delay(50);
        }
      }
      
      readStringg=""; //clears variable for new input
      joyRButtonState="";
      joyRX="";
      joyRY="";
    } else {     
      readStringg += c; //makes the string readString
    }
  }
}
