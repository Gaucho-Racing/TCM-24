#include <Arduino.h>
#include "FlexCAN_T4.h"
#include "send_nodes.h"
#include <iostream>
#define HWRSerial Serial3

leaked_nodes test;
get_nodes result;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary; //Depends on what board u test on ig but prob just use this for now
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary;
//FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> canData;
CAN_message_t msgPrimary;
CANFD_message_t msgData;

#define JuanMbps 1000000
#define AteMbps 8000000


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

CAN_message_t msg;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.begin(115200);
  HWRSerial.begin(115200, SERIAL_8N1);
  
  //gen_random(768, test.only_nodes);
  result.take_nodes(test.get_only_nodes());

  // Initialize the CAN bus
  canPrimary.begin();
  canPrimary.setBaudRate(JuanMbps);
  // canData.begin();
  // CANFD_timings_t config;
  // config.clock = CLK_24MHz;
  // config.baudrate = JuanMbps;
  // config.baudrateFD = AteMbps;
  // config.propdelay = 190;
  // config.bus_length = 1;
  // config.sample = 70;
  // canData.setBaudRate(config);
}

void loop() {
  if(canPrimary.read(msg)){
    Serial.println(msg.id, HEX);
  }
  test.concact_nodes(inverter, ecu, wheel1, wheel2, wheel3, wheel4, IMU, GPS, Pedals, ACU, BCM, Dash, msg.buf);
  if(HWRSerial.available() > 0){
    //gen_random(768, test.only_nodes);
    char chtemp = HWRSerial.read();
    //Serial.print(chtemp);
    if(chtemp == 0x06){
      Serial.println("\n ack recieved");
      HWRSerial.write(test.get_only_nodes(), 768);
      Serial.println("sent");
      //added this to check if it was a problem with flush 
      //this might not be important as there is no reason as to wait for flush as serial write sends all the data in onyl nodes. 
      //HWRSerial.flush();
    }
  }
}