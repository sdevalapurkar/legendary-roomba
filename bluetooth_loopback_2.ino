void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  if(Serial.available()){
    Serial1.print((char)Serial.read());
  }

  if(Serial1.available()){
    Serial1.print((char)Serial1.read());
  }
}
