void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

int bytesStored = 0;
char buff[31]; // init buffer
char newBuff[31]; // output buffer
bool isEnded = false;

void loop() {
  // read from UART0, send to UART1
  if (Serial.available()) {
    bytesStored += Serial.readBytes(buff, 30);
    Serial1.write(buff);
    memset(buff, 0, 31);
  }

  // read from UART1, send to UART0
  if (Serial1.available()) {
    Serial1.readBytes(newBuff, 30);

    int i = 0;
    for (i = 0; i < 31; i++) {
      if (newBuff[i] == 0X04) {
        isEnded = true;
      }
    }

    Serial.write(newBuff);
    Serial.write("\n");
    memset(newBuff, 0, 31);

    if (isEnded) {
      Serial.print("Process killed. ");
      Serial.print("Total bytes sent/recv: ");
      Serial.print(bytesStored);
      Serial.end();
    }
  }
}
