#include <Servo.h>
#include "scheduler.h"

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
uint8_t idle_pin = 7;
 
void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  servoY.attach(servoY_pin);
  servoX.attach(servoX_pin);
  servoY.write(initial_positionY);
  servoX.write(initial_positionX);

  Scheduler_Init();
 
  Scheduler_StartTask(0, 10, readValues);
  Scheduler_StartTask(1, 5, sendLaser);
  Scheduler_StartTask(0, 50, moveServo);
}

// idle task
void idle(uint32_t idle_period)
{
  // this function can perform some low-priority task while the scheduler has nothing to run.
  // It should return before the idle period (measured in ms) has expired.  For example, it
  // could sleep or respond to I/O.
 
  // example idle function that just pulses a pin.
  digitalWrite(idle_pin, HIGH);
  delay(idle_period);
  digitalWrite(idle_pin, LOW);
}

void sendLaser() {
  if (joyRButtonState.toInt() == 0) {
    if (toggle) {
      digitalWrite(laserPin, HIGH);   // set the LED on
      toggle = !toggle;
    } else {
      digitalWrite(laserPin, LOW);    // set the LED off
      toggle = !toggle;
    }
  }
}

void moveServo() {
  if (joyRX.toInt() < 300) {
    if (initial_positionX < 10) { 
    } else { 
      initial_positionX = initial_positionX - 1; 
      servoX.write (initial_positionX); 
    } 
  } 

  if (joyRX.toInt() > 700) {
    if (initial_positionX > 180) {  
    } else {
      initial_positionX = initial_positionX + 1;
      servoX.write (initial_positionX);
    }
  }

  if (joyRY.toInt() < 300){
    if (initial_positionY < 10) { 
    } else { 
      initial_positionY = initial_positionY - 1; 
      servoY.write (initial_positionY);
    } 
  } 

  if (joyRY.toInt() > 700){
    if (initial_positionY > 180){  
    } else{
      initial_positionY = initial_positionY + 1;
      servoY.write (initial_positionY);
    }
  }
}

void readValues() {
  if (Serial1.available())  {
    char c = Serial1.read();  //gets one byte from serial buffer
    
    if (c == '*') {
      ind1 = readStringg.indexOf(',');  //finds location of first ,
      joyRButtonState = readStringg.substring(0, ind1);   //captures first data String
      ind2 = readStringg.indexOf(',', ind1+1 );   //finds location of second ,
      joyRX = readStringg.substring(ind1+1, ind2);   //captures second data String
      ind3 = readStringg.indexOf(',', ind2+1 );
      joyRY = readStringg.substring(ind2+1, ind3);
      
      readStringg=""; //clears variable for new input
    } else {
      readStringg += c; //makes the string readString
    }
  }
}

void loop() {
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }
}

int main()
{
  init();
  setup();
 
  for (;;)
  {
    loop();
  }
  for (;;);
  return 0;
}
