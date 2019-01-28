#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 90 degrees (center or 1500 us) to 0 degrees (left or 1000 us)
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(14);                       // waits 14ms for the servo to reach the position
  }

  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees (1000 us to 2000 us)
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(14);                       // waits 14ms for the servo to reach the position
  }

  for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 90 degrees (2000 us to 1500 us)
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(14);                       // waits 14ms for the servo to reach the position
  }
}
