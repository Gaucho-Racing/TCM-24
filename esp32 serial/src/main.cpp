#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}
//rx is 34, tx is 35
void loop() {
  delay(10);
  byte data[768];
  
  int i = 0;
  Serial.write(0x06);
  if(Serial.available() > 0){
    Serial.readBytes(data, 768);
    for(int i = 0; i < 768; i++){
      Serial.print(data[i], HEX);
    }
  }
}