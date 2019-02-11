#include <LiquidCrystal.h>
# undef main

const int rs = 44, en = 45, d4 = 40, d5 = 41, d6 = 38, d7 = 39;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int minutes = 0;
int seconds = 0;
int hSeconds = 0;

ISR(TIMER3_COMPA_vect)
{
  digitalWrite(52, 1);

  hSeconds++;
  
  if (hSeconds >= 1000) {
    hSeconds = 0;
    seconds++;
  }
  
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
  }
  
  if (minutes >= 60) {
    minutes = 0;
    seconds = 0;
    hSeconds = 0;
  }

  digitalWrite(52, 0);
}

int main() {
  init();

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("      INIT    ");

  pinMode(52, OUTPUT);
  cli();
  
  // clear timer config
  TCCR3A = 0;
  TCCR3B = 0;
  
  // set to CTC (mode 4)
  TCCR3B |= (1<<WGM32);
  
  // set CS10 bit so timer runs at clock speed:
  TCCR3B |= B00000011;
  
  // set timer target
  OCR3A = 250;

  // enable interrupt A for timer 3
  TIMSK3 |= (1<<OCIE3A);
  
  sei();
  delay(1000);

  while (1) {
    lcd.clear();
    lcd.home();
    lcd.print(String(minutes) + ":" + String(seconds) + ":" + String(hSeconds));
    delay(200);
  }
}
