#include <Arduino.h>
#include "send_nodes.h"
bool request;
get_nodes result;
unsigned long requestTime;
bool clear;
void setup() {
  Serial.setRxBufferSize(1024);
  Serial.begin(115200);
  request = false;
  clear = true;
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
    Serial.print(Serial.available());
    if(clear){
      clear = false;
      while(Serial.available()){
        Serial.read();
      }
    }
  }
  if(Serial.available() >= 768){
    Serial.readBytes(data, 768);
    clear = true;
    request = false;
    result.take_nodes(data);
    Serial.println(result.EM[3]);
  }
}