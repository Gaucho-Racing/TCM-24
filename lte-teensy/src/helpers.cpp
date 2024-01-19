#include "send_nodes.h"

void leaked_nodes::concact_nodes(byte inverter[5][8], byte ecu[17][8], byte wheel1[5][8],byte wheel2[5][8],
                    byte wheel3[5][8],byte wheel4[5][8], byte IMU[3][8], byte GPS[4][8], byte Pedals[2][8], 
                    byte ACU[40][8], byte BCM[8], byte Dash[3][8], byte EM[8])
                    {
    byte inverter1D[40];
    byte ecu1D[136];
    byte wheel11D[40];
    byte wheel21D[40];
    byte wheel31D[40];
    byte wheel41D[40];
    byte IMU1D[24];
    byte GPS1D[32];
    byte Pedals1D[16];
    byte ACU1D[320];
    //bcm is already 1D
    byte Dash1D[24];
    //em is already 1D
    convert2D(inverter, 5, 8, inverter1D);
    convert2D(ecu, 17, 8, ecu1D);
    convert2D(wheel1, 5, 8, wheel11D);
    convert2D(wheel2, 5, 8, wheel21D);
    convert2D(wheel3, 5, 8, wheel31D);
    convert2D(wheel4, 5, 8, wheel41D);
    convert2D(IMU, 3, 8, IMU1D);
    convert2D(GPS, 4, 8, GPS1D);
    convert2D(Pedals, 2, 8, Pedals1D);
    convert2D(ACU, 40, 8, ACU1D);
    //bcm
    convert2D(Dash, 3, 8, Dash1D);
    //em
    Serial.println("Inverter");
    for(int i = 0; i < 40; i++){
        Serial.print(inverter1D[i]);
    }
    Serial.println("");
    memcpy(only_nodes, inverter1D, 40);
    memcpy(only_nodes + 40, ecu1D, 136);
    memcpy(only_nodes + 176, wheel11D, 40);
    memcpy(only_nodes + 216, wheel21D, 40);
    memcpy(only_nodes + 256, wheel31D, 40);
    memcpy(only_nodes + 296, wheel41D, 40);
    memcpy(only_nodes + 336, IMU1D, 24);
    memcpy(only_nodes + 360, GPS1D, 32);
    memcpy(only_nodes + 392, Pedals1D, 16);
    memcpy(only_nodes + 408, ACU1D, 320);
    memcpy(only_nodes + 728, BCM, 8);
    memcpy(only_nodes + 736, Dash1D, 24);
    memcpy(only_nodes + 760, EM, 8);
}

void get_nodes::take_nodes(byte nodes[768]){
        for(int i = 0; i < 40; i ++){
            inverter[i] = nodes[i];
        }
        for(int i = 0; i < 138; i ++){
            ecu[i] = nodes[i + 40];
        }
        for(int i = 0; i < 40; i ++){
            wheel1[i] = nodes[i + 176];
        }
        for(int i = 0; i < 40; i ++){
            wheel2[i] = nodes[i + 216];
        }
        for(int i = 0; i < 40; i ++){
            wheel3[i] = nodes[i + 256];
        }
        for(int i = 0; i < 40; i ++){
            wheel4[i] = nodes[i + 296];
        }
        for(int i = 0; i < 24; i ++){
            IMU[i] = nodes[i + 336];
        }
        for(int i = 0; i < 32; i ++){
            GPS[i] = nodes[i + 360];
        }
        for(int i = 0; i < 16; i ++){
            Pedals[i] = nodes[i + 392];
        }
        for(int i = 0; i < 320; i ++){
            ACU[i] = nodes[i + 408];
        }
        for(int i = 0; i < 8; i ++){
            BCM[i] = nodes[i + 728];
        }
        for(int i = 0; i < 24; i ++){
            Dash[i] = nodes[i + 736];
        }
        for(int i = 0; i < 8; i ++){
            EM[i] = nodes[i + 760];
        }
}

//this is fucking black magic
void leaked_nodes::convert2D(byte arr2D[][8], int row, int col, byte arr1D[]){
    int res = row * col;
    for (int i = 0; i < res; ++i) {
        arr1D[i] = *(&arr2D[0][0] + i);
    }
}

void gen_random(const int len, byte *res) {
    static const char alphanum[] =
        "0123456789"
        "abcdef";
    for (int i = 0, p = 0; i < len; ++i) {
        res[i] = alphanum[int(random(0, sizeof(alphanum) - 1))];
        }
}