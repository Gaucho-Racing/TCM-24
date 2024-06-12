#include "send_nodes.h"

//converts all the nodes into one list of all of the bytes to send over serial
void leaked_nodes::concact_nodes(byte inverter[5][8], byte vdm[6][8], byte wheel1[5][8],byte wheel2[5][8],
                    byte wheel3[5][8],byte wheel4[5][8], byte IMU[3][8], byte GPS[4][8], byte Pedals[2][8], 
                    byte ACU[50][8], byte TCM[8], byte Dash[3][8], byte EM[2][8])
                    {
    byte inverter1D[40];
    byte vdm1D[48];
    byte wheel11D[40];
    byte wheel21D[40];
    byte wheel31D[40];
    byte wheel41D[40];
    byte IMU1D[24];
    byte GPS1D[32];
    byte Pedals1D[16];
    byte ACU1D[400];
    //bcm is already 1D
    byte Dash1D[24];
    byte EM1D[16];
    convert2D(inverter, 5, 8, inverter1D);
    convert2D(vdm, 6, 8, vdm1D);
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
    convert2D(EM, 2, 8, EM1D);
    memcpy(only_nodes, inverter1D, 40);
    memcpy(only_nodes + 40, vdm1D, 48);
    memcpy(only_nodes + 88, wheel11D, 40);
    memcpy(only_nodes + 128, wheel21D, 40);
    memcpy(only_nodes + 168, wheel31D, 40);
    memcpy(only_nodes + 208, wheel41D, 40);
    memcpy(only_nodes + 248, IMU1D, 24);
    memcpy(only_nodes + 272, GPS1D, 32);
    memcpy(only_nodes + 304, Pedals1D, 16);
    memcpy(only_nodes + 320, ACU1D, 400);
    memcpy(only_nodes + 720, TCM, 8);
    memcpy(only_nodes + 728, Dash1D, 24);
    memcpy(only_nodes + 752, EM, 16);
}

//sorts the array back into what it was meant to be originally. 
void get_nodes::take_nodes(byte nodes[768]){
    memcpy(inverter, nodes, 40);
    memcpy(vdm, nodes + 40, 48);
    memcpy(wheel1, nodes + 88, 40);
    memcpy(wheel2, nodes + 128, 40);
    memcpy(wheel3, nodes + 168, 40);
    memcpy(wheel4, nodes + 208, 40);
    memcpy(IMU, nodes + 248, 24);
    memcpy(GPS,nodes + 272, 32);
    memcpy(Pedals, nodes + 304, 16);
    memcpy(ACU, nodes + 320, 400);
    memcpy(TCM, nodes + 720, 8);
    memcpy(Dash, nodes + 728, 24);
    memcpy(EM, nodes + 752, 16);
}

//this is fucking black magic nvm i now understand it. it cool
void leaked_nodes::convert2D(byte arr2D[][8], int row, int col, byte arr1D[]){
    int res = row * col;
    for (int i = 0; i < res; ++i) {
        arr1D[i] = *(&arr2D[0][0] + i);
    }
}

//actual helper function for generating random alpha numeric data
void gen_random(const int len, byte *res) {
    static const char alphanum[] =
        "0123456789"
        "abcdef";
    for (int i = 0, p = 0; i < len; ++i) {
        res[i] = alphanum[int(random(0, sizeof(alphanum) - 1))];
        }
}