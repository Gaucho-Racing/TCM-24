#include <Arduino.h>
#include "send_nodes.h"

get_nodes result;
void setup() {
  Serial.begin(115200);
}
//rx is 34, tx is 35
void loop() {
  delay(1000);
  byte data[768];

  
  int i = 0;
  Serial.write(0x06);
  if(Serial.available() > 0){
    //added this to check all of the things coming through 
    Serial.print(Serial.read());
    //Serial.readBytes(data, 768);
    /*
    for(int i = 0; i < 768; i++){
      Serial.print(data[i], HEX);
    }
    */
   //result.take_nodes(data);
   for(int i = 0; i < 768; i ++){
    Serial.print(data[i], HEX);
   }
  }
}