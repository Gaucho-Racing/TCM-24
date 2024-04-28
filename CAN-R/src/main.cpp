#include <Arduino.h>
#include "FlexCAN_T4.h"
#include "send_nodes.h"
#include "icanflex.h"
#include "Nodes.h"
#include <iostream>
#define HWRSerial Serial3

leaked_nodes test;
get_nodes result;
FlexCAN_T4<CAN1, RX_SIZE_1024, TX_SIZE_16> can1;
CAN_message_t msg;
CAN_message_t msg1;

#define JuanMbps 1000000

iCANflex* Car;


byte inverter[5][8] = {0x85}; 
byte ecu[17][8] = {0x66}; 
byte wheel1[5][8] = {0x67};
byte wheel2[5][8] = {0x68};
byte wheel3[5][8]= {0x69};
byte wheel4[5][8]= {0x70};
byte IMU[3][8] = {0x71};
byte GPS[4][8] = {0x72};
byte Pedals[2][8] = {0x00};
byte ACU[40][8] = {0x74};
byte BCM[8] = {0x75};
byte Dash[3][8] = {0x76};
byte EM[8] = {0x77};
byte random_Data[768];

void setup() {
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin(115200);
  delay(100);
  //Serial.begin(115200);
  HWRSerial.begin(115200, SERIAL_8N1);
  
  Car = new iCANflex();
  Car->begin();

  //gen_random(768, test.only_nodes);
  result.take_nodes(test.get_only_nodes());

  // Initialize the CAN bus
  can1.begin();
  can1.setBaudRate(JuanMbps);
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
  //Car->readData(msg);
  can1.read(msg);
  if(msg.id == 0xC8){
  for(int i =0; i < 8; i++){
    //Serial.print(msg.buf[i], HEX);
    Pedals[0][i] = msg.buf[i];
  }
  }
  Serial.println();
  //Serial.println(Car->PEDALS.getAPPS1());
  //Serial.println(Car->PEDALS.getAPPS2());
  //Serial.println(Car->PEDALS.getBrakePressureF());
  //Serial.println(Car->PEDALS.getBrakePressureR());
  //Serial.println("test");
  // for(int i = 0; i < 8; i ++){
  //   Pedals[0][i] = msg.buf[i];
  //   Serial.println(Pedals[0][i]);
  // }
  test.concact_nodes(inverter, ecu, wheel1, wheel2, wheel3, wheel4, IMU, GPS, Pedals, ACU, BCM, Dash, EM);
  //Car->PEDALS.wipe();
  if(HWRSerial.available() > 0){
    
    //gen_random(768, test.only_nodes);
    char chtemp = HWRSerial.read();
    //Serial.println("in loop");
    //Serial.print(chtemp);
    if(chtemp == 0x06){
      //Serial.println("\n ack recieved");
      digitalWrite(LED_BUILTIN, HIGH);
      HWRSerial.write(test.get_only_nodes(), 768);
      msg1.id = 0x69420;
      msg1.flags.extended = true;
      msg1.buf[0] = 0xFF;
      can1.write(msg1);
      
      //Serial.println("sent");
      //added this to check if it was a problem with flush 
      //this might not be important as there is no reason as to wait for flush as serial write sends all the data in onyl nodes. 
      //HWRSerial.flush();
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}