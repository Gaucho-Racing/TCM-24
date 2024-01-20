#include <Arduino.h>
#include "send_nodes.h"
bool request;
get_nodes result;
unsigned long requestTime;
void setup() {
  Serial.setRxBufferSize(1024);
  Serial.begin(115200);
  request = false;
}
void loop() {
  
  byte data[768];
  if(!request){
    Serial.write(0x06);
    requestTime = millis();
    request = true;
  }
  unsigned long now = millis();
  if(now - requestTime > 1000){
    Serial.write(0x06);
    requestTime = now;
  }
  if(Serial.available() >= 768){
    Serial.readBytes(data, 768);
    /*
    for(int i = 0; i < 768; i++){
      Serial.print(data[i], HEX);
    }
    */
    request = false;
    result.take_nodes(data);
    for(int i = 0; i < 40; i++){
      Serial.print(result.wheel3[i], HEX);
    }
  }
}