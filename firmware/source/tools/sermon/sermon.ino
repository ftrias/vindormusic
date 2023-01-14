void setup() {
  Serial1.begin(57600);
  Serial.begin(57600);
}

void loop() {
  if (Serial1.available()>0) {
    int b = Serial1.read();
    Serial.print((char)b);
  }
  else {
    delay(100);
    Serial.print(".");
  }
}
