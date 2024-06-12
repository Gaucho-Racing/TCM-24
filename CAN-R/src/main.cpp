#include <Arduino.h>
#include "FlexCAN_T4.h"
#include "send_nodes.h"
#include "Nodes.h"
//#include <iostream>
#define HWRSerial Serial1

leaked_nodes test;
get_nodes result;
bool start = false;
unsigned long canFill;

#define JuanMbps 1000000

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can_primary;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can_data;
CAN_message_t msg;
CAN_message_t msg2;
Inverter DTI = Inverter(22, can_primary);
VDM ECU = VDM(can_primary, can_data);
Wheel WFL = Wheel(can_data, WHEEL_FL);
Wheel WFR = Wheel(can_data, WHEEL_FR);
Wheel WRL = Wheel(can_data, WHEEL_RL);
Wheel WRR = Wheel(can_data, WHEEL_RR);
GPS GPS1 = GPS(can_data);
Pedals PEDALS = Pedals(can_primary);
ACU ACU1 = ACU(can_primary);
TCM TCM1 = TCM(can_data);
Dash DASHBOARD = Dash(can_primary);
Energy_Meter ENERGY_METER = Energy_Meter(can_primary);
SteeringWheel STEERING_WHEEL = SteeringWheel(can_primary);
byte IMU[3][8] = {0x00};
int i = 0;

void setup() {
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin(115200);
  delay(100);
  HWRSerial.begin(115200, SERIAL_8N1);
  can_primary.begin();
  can_primary.setBaudRate(1000000);
  msg.flags.extended = 1;
  can_data.begin();
  can_data.setBaudRate(1000000);
}

void loop() {
  if(can_primary.read(msg)){
    DTI.receive(msg.id, msg.buf);
    ECU.receive(msg.id, msg.buf);
    PEDALS.receive(msg.id, msg.buf);
    ACU1.receive(msg.id, msg.buf);
    TCM1.receive(msg.id, msg.buf);
    DASHBOARD.receive(msg.id, msg.buf);
    ENERGY_METER.receive(msg.id, msg.buf);
    STEERING_WHEEL.receive(msg.id, msg.buf);
  }
  if(can_data.read(msg2)){
    WFL.receive(msg.id, msg.buf);
    WFR.receive(msg.id, msg.buf);
    WRL.receive(msg.id, msg.buf);
    WRR.receive(msg.id, msg.buf);
    GPS1.receive(msg.id, msg.buf);
  }
  //code to print out Pedals from NODES 
  test.concact_nodes(DTI.data, ECU.data, WFR.data, WFL.data,  WRR.data, WRL.data, IMU, GPS1.data, PEDALS.data, ACU1.data, TCM1.data, DASHBOARD.data, ENERGY_METER.data);
  result.take_nodes(test.only_nodes);
  //Serial.println();
  if(!start){
    canFill = millis();
    start = true;
  }
  //gives some start up time to fill the can shit 
  //if(millis() > (canFill + 100)){
    
    //Serial.println(HWRSerial.available());
    if(HWRSerial.available() > 0){
      
      //gen_random(768, test.only_nodes);
      char chtemp = HWRSerial.read();
      //Serial.println("in loop");
      Serial.print(chtemp);
      if(chtemp == 0x06){
        Serial.println("\n ack recieved");
        digitalWrite(LED_BUILTIN, HIGH);
        HWRSerial.write(test.get_only_nodes(), 768);
        msg.id = 0x69420;
        msg.flags.extended = true;
        msg.buf[0] = 0xFF;
        can_primary.write(msg);
        
        //Serial.println("sent");
        //added this to check if it was a problem with flush 
        //this might not be important as there is no reason as to wait for flush as serial write sends all the data in onyl nodes. 
        //HWRSerial.flush();
      }
    }
  digitalWrite(LED_BUILTIN, LOW);
}