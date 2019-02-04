void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(52, OUTPUT);
  pinMode(50, OUTPUT);
}

void loop(){
  if(Serial.available()){
    digitalWrite(52, HIGH);
    Serial1.print((char)Serial.read());
    digitalWrite(52, LOW);
  }

  if(Serial1.available()){
    digitalWrite(50,HIGH);
    Serial.print((char)Serial1.read());
    digitalWrite(50, LOW);
  }
}
