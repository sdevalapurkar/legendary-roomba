#include <Servo.h>
#include <LiquidCrystal.h>

Servo servoX;
Servo servoY;

int joyRX = A1;
int joyRY = A0;
int joyRbutton = 23;
int x_pos;
int y_pos;
int servoY_pin = 8;
int servoX_pin = 9;
int lightPin = A15; 
int lightSensorValue = 0;
int initial_positionY = 80;
int initial_positionX = 100;
int joyRbuttonState = 0;
int laserPin = 2;
boolean toggle = true;

const int rs = 44, en = 45, d4 = 40, d5 = 41, d6 = 38, d7 = 39;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup () {
  Serial.begin (9600) ;
  servoY.attach (servoY_pin ) ; 
  servoX.attach (servoX_pin ) ; 
  servoY.write (initial_positionY);
  servoX.write (initial_positionX);
  pinMode (joyRX, INPUT) ;                     
  pinMode (joyRY, INPUT) ; 
  pinMode (joyRbutton, INPUT_PULLUP); 
  pinMode (laserPin, OUTPUT); 
  pinMode (lightPin, INPUT);
  //Start the LCD object.
  lcd.begin(16, 2);
  //Start with a clear LCD screen.
  lcd.clear();                 
}

void loop ( ) {
  lightSensorValue = analogRead(lightPin); // read the value from the sensor
  Serial.println(lightSensorValue); 

  lcd.setCursor(0, 0);
  if (lightSensorValue > 900){
    lcd.print("HIT             ");
  } else{
    lcd.print("ALIVE            ");
  }

  x_pos = analogRead (joyRX) ;  
  y_pos = analogRead (joyRY) ;
  joyRbuttonState = digitalRead(joyRbutton);
  
  if (joyRbuttonState == LOW){
    delay(50);  //delay for debounce
    if(toggle){
      digitalWrite(laserPin, HIGH);   // set the LED on
      toggle = !toggle;
    } else{
      digitalWrite(laserPin, LOW);    // set the LED off
      toggle = !toggle;
    }
  }

  lcd.setCursor(0,1);
  if (x_pos < 300){
    if (initial_positionX < 10) { 
    } else { 
      initial_positionX = initial_positionX - 1; 
      servoX.write (initial_positionX);
      lcd.print("RIGHT           "); 
      delay(50); 
    } 
  } 
  
  if (x_pos > 700){
    if (initial_positionX > 180){  
    } else {
      initial_positionX = initial_positionX + 1;
      servoX.write (initial_positionX);
      lcd.print("LEFT             ");
      delay(50);
    }
  }
  
  if (y_pos < 300){
    if (initial_positionY < 10) { 
    } else { 
      initial_positionY = initial_positionY - 1; 
      servoY.write (initial_positionY);
      lcd.print("UP               ");
      delay(50); 
      } 
   } 
  
  if (y_pos > 700){
    if (initial_positionY > 180){  
    } else{
      initial_positionY = initial_positionY + 1;
      servoY.write (initial_positionY);
      lcd.print("DOWN             ");
      delay(50);
    }
  }
}
