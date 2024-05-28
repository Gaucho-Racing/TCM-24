#ifndef SEND_NODES
#define SEND_NODES
#include <Arduino.h>

struct leaked_nodes{
    public:
        byte *get_only_nodes(){return only_nodes;}
        void concact_nodes(byte inverter[5][8], byte vdm[6][8], byte wheel1[5][8],byte wheel2[5][8],byte wheel3[5][8],byte wheel4[5][8], byte IMU[3][8], byte GPS[4][8], byte Pedals[2][8], byte ACU[50][8], byte BCM[8], byte Dash[3][8], byte EM[2][8]);
        byte only_nodes[768];
    private:
        void convert2D(byte arr2D[][8], int row, int col, byte arr1D[]);
        //for testing purposes have only nodes in public

};

struct get_nodes{
    byte inverter[44];
    byte vdm[52];
    byte wheel1[44];
    byte wheel2[44];
    byte wheel3[44];
    byte wheel4[44];
    byte IMU[28];
    byte GPS[36];
    byte Pedals[20];
    byte ACU[404];
    byte TCM[12];
    byte Dash[28];
    byte EM[20];
    void take_nodes(byte nodes[768]);
};

void gen_random(const int len, byte *res);


#endif
