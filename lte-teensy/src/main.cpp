#include <Arduino.h>
#include "send_nodes.h"
#include <iostream>
#define HWRSerial Serial1

leaked_nodes test;
get_nodes result;

byte inverter[5][8] = {0x85}; 
byte ecu[17][8] = {0x66}; 
byte wheel1[5][8] = {0x67};
byte wheel2[5][8] = {0x68};
byte wheel3[5][8]= {0x69};
byte wheel4[5][8]= {0x70};
byte IMU[3][8] = {0x71};
byte GPS[4][8] = {0x72};
byte Pedals[2][8] = {0x73};
byte ACU[40][8] = {0x74};
byte BCM[8] = {0x75};
byte Dash[3][8] = {0x76};
byte EM[8] = {0x77};
byte random_Data[768];

void setup(){
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1);
  test.concact_nodes(inverter, ecu, wheel1, wheel2, wheel3, wheel4, IMU, GPS, Pedals, ACU, BCM, Dash, EM);
  //gen_random(768, test.only_nodes);
  result.take_nodes(test.get_only_nodes());
  
}

void loop(){

  //this was for testing 
  /*
  delay(1000);
  Serial.println("\nresult");
  for(int i = 0; i < 40; i++){
    Serial.print(result.wheel1[i], HEX);
  };
  Serial.println("\nsource");
  for(int i = 0; i < 40; i++){
    Serial.print(test.only_nodes[i + 176], HEX);
  }
  */
  //Serial.print(Serial1.read());
  if(Serial1.available() > 0){
    //gen_random(768, test.only_nodes);
    char chtemp = Serial1.read();
    //Serial.print(chtemp);
    if(chtemp == 0x06){
      Serial.println("\n ack recieved");
      Serial1.write(test.get_only_nodes(), 768);
      //added this to check if it was a problem with flush 
      //this might not be important as there is no reason as to wait for flush as serial write sends all the data in onyl nodes. 
      Serial1.flush();
    }
  }
  /*
  delay(3000);
  byte* data = test.get_only_nodes();
  delay(2000);
  for(int i = 0; i < 40; i++){
    Serial.print(data[i + 176], HEX);
  }
  
  Serial.println("\ndecoded");
  for(int i = 0; i < 40; i++){
    Serial.print(result.wheel1[i], HEX);
  }
  Serial.println();
  */
}
/*

void setup(){
   test_data.myArray[2] = 50;
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1);
   
}

 void send(const test* data){
    HWRSerial.write((const char*) data, sizeof(struct test));
 }

 void loop(){
   send(&test_data);
    if(Serial1.available() > 0){
    char chtemp = Serial1.read();
    //Serial.print(chtemp);
  }
 }
 */


