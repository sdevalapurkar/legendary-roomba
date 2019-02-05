#include <LiquidCrystal.h>
#include "scheduler.h"

int joyRX = A1;
int joyRY = A0;
int joyRbutton = 23;
int x_pos;
int y_pos;
int lightPin = A15; 
int lightSensorValue = 0;
int initial_positionY = 80;
int initial_positionX = 100;
int joyRbuttonState = 0;
int laserPin = 2;
boolean toggle = true;
int sensorValue[3];
uint8_t idle_pin = 7;

const int rs = 44, en = 45, d4 = 40, d5 = 41, d6 = 38, d7 = 39;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


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


void setup () {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode (joyRX, INPUT);                     
  pinMode (joyRY, INPUT); 
  pinMode (joyRbutton, INPUT_PULLUP); 
  pinMode (lightPin, INPUT);
  pinMode (7, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();

  Scheduler_Init();

  Scheduler_StartTask(0, 50, readSensors);
  Scheduler_StartTask(0, 200, sendValues);
  Scheduler_StartTask(0, 50, setCursorZero);
  Scheduler_StartTask(0, 200, checkLightSensor);
  Scheduler_StartTask(0, 50, setCursorOne);
  Scheduler_StartTask(0, 50, checkJoyRAxis);
}


void readSensors(){
  lightSensorValue = analogRead(lightPin);
  x_pos = analogRead(joyRX);  
  y_pos = analogRead(joyRY);
  
  sensorValue[0] = digitalRead(joyRbutton);
  sensorValue[1] = analogRead(joyRX);  
  sensorValue[2] = analogRead(joyRY);
}


void sendValues(){
  for (int k=0; k<3; k++){
    Serial1.print(sensorValue[k]);
    if (k<2){
      Serial1.print(',');
    }
  }
  Serial1.print('*');
  Serial1.println();
}


void checkLightSensor(){
  if (lightSensorValue > 900){
    lcdPrint("HIT             ");
  } else{
    lcdPrint("ALIVE            ");
  }
}


void lcdPrint(String text){
  lcd.print(text);
}


void checkJoyRAxis(){
  if (x_pos < 300){
    if (initial_positionX < 10) { 
    } else { 
      lcdPrint("RIGHT           "); 
//      delay(50); 
    } 
  } 
  
  if (x_pos > 700){
    if (initial_positionX > 180){  
    } else {
      lcdPrint("LEFT             ");
//      delay(50);
    }
  }

  if (y_pos < 300){
    if (initial_positionY < 10) { 
    } else { 
      lcdPrint("UP               ");
//      delay(50); 
      } 
   } 
  
  if (y_pos > 700){
    if (initial_positionY > 180){  
    } else{
      lcdPrint("DOWN             ");
//      delay(50);
    }
  }
}


void setCursorZero(){
  lcd.setCursor(0, 0);
}


void setCursorOne(){
  lcd.setCursor(0, 1);
}


void loop() {

  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }
  
//  readSensors();
//  sendValues();
//  
//  setCursors(0, 0);
//  checkLightSensor();
//
//  setCursors(0,1);
//  checkJoyRAxis();
}

int main() {
  init();
  setup();
 
  for (;;)
  {
    loop();
  }
  for (;;);
  return 0;
}
