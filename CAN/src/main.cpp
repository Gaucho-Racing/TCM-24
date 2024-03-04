#include <Arduino.h>
#include "FlexCAN_T4.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary; //Depends on what board u test on ig but prob just use this for now
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary;
//FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> canData;
CAN_message_t msgPrimary;
CANFD_message_t msgData;

#define JuanMbps 1000000
#define AteMbps 8000000

byte suspensionPos = 0;
unsigned int wheelSpeed = 0;
unsigned int tirePressure = 0;
unsigned int APPS1 = 0;
unsigned int APPS2 = 0;
void setup() {
  Serial.begin(115200);
  delay(1000);

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
  msgPrimary.id = 0x2AB;
  msgPrimary.len = 8;
  msgPrimary.buf[0] = (byte)(APPS1 >> 8);
  msgPrimary.buf[1] = (byte)(APPS1);
  msgPrimary.buf[2] = (byte)(APPS2 >> 8);
  msgPrimary.buf[3] = (byte)(APPS2);
  msgPrimary.buf[4] = (byte)(wheelSpeed >> 8);
  msgPrimary.buf[5] = (byte)wheelSpeed;
  msgPrimary.buf[6] = (byte)(tirePressure >> 8);
  msgPrimary.buf[7] = (byte)tirePressure;
  canPrimary.write(msgPrimary);
  Serial.println("Frame sent!");

  delay(1000);  // Adjust the delay according to your needs
  APPS1 += 1;
  if(APPS1 > 100){
    APPS1 = 0;
  }
  APPS2 += 10;
  if(APPS2 > 100){
    APPS2 = 0;
  }
  suspensionPos = 0;
  wheelSpeed = 0;
  tirePressure = 0;
}