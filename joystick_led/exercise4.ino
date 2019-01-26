 #include "Filters.h"

 int ledPin = 13;
 int joyPinX = 0;                 // slider variable connecetd to analog pin 0
 int joyPinY = 1;                 // slider variable connecetd to analog pin 1
 int valueX = 0;
 int valueY = 0;
 int brightness = 0;
 int fadeAmount = 5;
  
 void setup() {
  pinMode(ledPin, OUTPUT);              // initializes digital pins 0 to 7 as outputs
  Serial.begin(9600);
 }

void loop() { 
  
 valueX = analogRead(joyPinX);            // reads the value of the potentiometer (value between 0 and 1023) 
 valueX = map(valueX, 512, 1023, 200, 0);     // scale it to use it with the servo (value between 0 and 180) 
 analogWrite(ledPin, brightness);
  
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;
  
  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }

  if (202 < valueX && valueX < 213){
    delay(30);
  } else{
    Serial.println(valueX);
    delay(valueX);
  }
  

 
}
