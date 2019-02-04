#include <LiquidCrystal.h>

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

const int rs = 44, en = 45, d4 = 40, d5 = 41, d6 = 38, d7 = 39;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup () {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode (joyRX, INPUT);                     
  pinMode (joyRY, INPUT); 
  pinMode (joyRbutton, INPUT_PULLUP); 
  pinMode (lightPin, INPUT);
  lcd.begin(16, 2);
  lcd.clear();                 
}


void readSensors(){
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
  delay(10);
}

void loop ( ) {
  readSensors();
  sendValues();
  
  lightSensorValue = analogRead(lightPin);
  x_pos = analogRead(joyRX);  
  y_pos = analogRead(joyRY);
  
  lcd.setCursor(0, 0);
  if (lightSensorValue > 900){
    lcd.print("HIT             ");
  } else{
    lcd.print("ALIVE            ");
  }

  lcd.setCursor(0,1);
  if (x_pos < 300){
    if (initial_positionX < 10) { 
    } else { 
      lcd.print("RIGHT           "); 
      delay(50); 
    } 
  } 
  
  if (x_pos > 700){
    if (initial_positionX > 180){  
    } else {
      lcd.print("LEFT             ");
      delay(50);
    }
  }
  
  if (y_pos < 300){
    if (initial_positionY < 10) { 
    } else { 
      lcd.print("UP               ");
      delay(50); 
      } 
   } 
  
  if (y_pos > 700){
    if (initial_positionY > 180){  
    } else{
      lcd.print("DOWN             ");
      delay(50);
    }
  }
}
