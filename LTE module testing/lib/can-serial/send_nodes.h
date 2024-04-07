#ifndef SEND_NODES
#define SEND_NODES
#include <Arduino.h>

struct leaked_nodes{
    public:
        byte *get_only_nodes(){return only_nodes;}
        void concact_nodes(byte inverter[5][8], byte ecu[17][8], byte wheel1[5][8],byte wheel2[5][8],byte wheel3[5][8],byte wheel4[5][8], byte IMU[3][8], byte GPS[4][8], byte Pedals[2][8], byte ACU[40][8], byte BCM[8], byte Dash[3][8], byte EM[8]);
        byte only_nodes[768];
    private:
        void convert2D(byte arr2D[][8], int row, int col, byte arr1D[]);
        //for testing purposes have only nodes in public

};

struct get_nodes{
    byte inverter[40];
    byte ecu[138];
    byte wheel1[40];
    byte wheel2[40];
    byte wheel3[40];
    byte wheel4[40];
    byte IMU[24];
    byte GPS[32];
    byte Pedals[16];
    byte ACU[320];
    byte BCM[8];
    byte Dash[24];
    byte EM[8];
    void take_nodes(byte nodes[768]);
};

void gen_random(const int len, byte *res);


#endif
