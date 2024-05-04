#include "FlexCAN_T4.h"
#include "Nodes.h"
#include "icanflex.h"

unsigned long lastRecieveTime=millis();

unsigned long stopwatch=0;
bool canSend = false;


iCANflex::iCANflex(){

}

bool iCANflex::begin() {   //Coordinate the magic CAN pixies to dance together 
    Serial.begin(115200);

    can_primary.begin();
    can_primary.setBaudRate(1000000);
    msg.flags.extended = 1;

    can_data.begin();
    CANFD_timings_t config;
    config.clock = CLK_24MHz;
    config.baudrate = 1000000;
    config.baudrateFD = 8000000;
    config.propdelay = 190;
    config.bus_length = 1;
    config.sample = 70;
    can_data.setBaudRate(config);
    return true;
}

int iCANflex::readData(CAN_message_t msg){    //Read data from the inverter or the BMS
    lastRecieveTime = millis();
    // if(!can_primary.read(msg)){ // fix this line I beg you - NO
    // //Serial.println("no");
    //     return 0;
    // }
    
    if(!msg.flags.extended){
        Serial.print("WHAT IS THIS SHIT???? I WANT AN EXTENDED FRAME!!!! COME ON DUDE -- ID: 0x");
        Serial.print(msg.id, HEX);
    }
    
    int count = 0;
    count += DTI.receive(msg.id, msg.buf);
    count += ECU.receive(msg.id, msg.buf);
    count += WFL.receive(msg.id, msg.buf);
    count += WFR.receive(msg.id, msg.buf);
    count += WRL.receive(msg.id, msg.buf);
    count += WRR.receive(msg.id, msg.buf);
    count += GPS1.receive(msg.id, msg.buf);
    count += PEDALS.receive(msg.id, msg.buf);
    count += ACU1.receive(msg.id, msg.buf);
    count += TCM1.receive(msg.id, msg.buf);
    count += DASHBOARD.receive(msg.id, msg.buf);
    count += ENERGY_METER.receive(msg.id, msg.buf);
    count += STEERING_WHEEL.receive(msg.id, msg.buf);
    if(count == 0) return 1;
    return count;
}

void iCANflex::rawData(INT32U *id, INT8U *buffer){*id=msg.id; buffer=msg.buf;}  //Gives back the raw Id and data array

void iCANflex::send(long OutId, byte stuff[]){    //Sends 8 bytes with that Id and that data shifted left all the way
    for (int i = 0; i < 8; i++)
        msg.buf[i] = stuff[i];
    msg.id = OutId;
    if(canSend){can_primary.write(msg);} //por favor mi amor
    else{Serial.println("YOU TRIED TO SEND????? - note this will prob appear when recieving from the bms actually nevermind ill fix that now");}
}