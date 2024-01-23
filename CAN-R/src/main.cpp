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
int wheelSpeed = 0;
byte tirePressure = 0;
CAN_message_t msg;

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
  if(canPrimary.read(msg)){
    Serial.println(msg.id, HEX);
  }
}