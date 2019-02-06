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

  // Scheduler testing pins
  pinMode(28, OUTPUT);
  digitalWrite(28, LOW);
  pinMode(29, OUTPUT);
  digitalWrite(29, LOW);
  pinMode(30, OUTPUT);
  digitalWrite(30, LOW);
  pinMode(31, OUTPUT);
  digitalWrite(31, LOW);
  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);

  Scheduler_Init();

  Scheduler_StartTask(0, 50, readSensors);
  Scheduler_StartTask(0, 200, sendValues);
  Scheduler_StartTask(0, 50, setCursorZero);
  Scheduler_StartTask(0, 200, checkLightSensor);
  Scheduler_StartTask(0, 50, setCursorOne);
  Scheduler_StartTask(0, 50, checkJoyRAxis);
}

// Testing pin 28 - ch1
void readSensors(){
  digitalWrite(28, HIGH);
  lightSensorValue = analogRead(lightPin);
  x_pos = analogRead(joyRX);  
  y_pos = analogRead(joyRY);
  
  sensorValue[0] = digitalRead(joyRbutton);
  sensorValue[1] = analogRead(joyRX);  
  sensorValue[2] = analogRead(joyRY);
  digitalWrite(28, LOW);
}

// Testing pin 29 - ch2
void sendValues(){
  digitalWrite(29, HIGH);
  for (int k=0; k<3; k++){
    Serial1.print(sensorValue[k]);
    if (k<2){
      Serial1.print(',');
    }
  }
  Serial1.print('*');
  Serial1.println();
  digitalWrite(29, LOW);
}

// Testing pin 30 - ch3
void checkLightSensor(){
  digitalWrite(30, HIGH);
  if (lightSensorValue > 900){
    lcdPrint("HIT             ");
  } else{
    lcdPrint("ALIVE            ");
  }
  digitalWrite(30, LOW);
}

void lcdPrint(String text){
  lcd.print(text);
}

// Testing pin 31 - ch4
void checkJoyRAxis(){
  digitalWrite(31, HIGH);
  if (x_pos < 300){
    if (initial_positionX < 10) { 
    } else { 
      lcdPrint("RIGHT           "); 
    } 
  } 
  
  if (x_pos > 700){
    if (initial_positionX > 180){  
    } else {
      lcdPrint("LEFT             ");
    }
  }

  if (y_pos < 300){
    if (initial_positionY < 10) { 
    } else { 
      lcdPrint("UP               ");
      } 
   } 
  
  if (y_pos > 700){
    if (initial_positionY > 180){  
    } else{
      lcdPrint("DOWN             ");
    }
  }
  digitalWrite(31, LOW);
}

// Testing pin 32 - ch5
void setCursorZero(){
  digitalWrite(32, HIGH);
  lcd.setCursor(0, 0);
  digitalWrite(32, LOW);
}

// Testing pin 33 - ch6
void setCursorOne(){
  digitalWrite(33, HIGH);
  lcd.setCursor(0, 1);
  digitalWrite(33, LOW);
}


void loop() {

  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }
  
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
