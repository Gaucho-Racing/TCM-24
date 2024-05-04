#include "Nodes.h"
#include "FlexCAN_T4.h"



#define USE_CAN_PRIMARY

#define INT8U byte
#define INT32U unsigned long

#ifndef icanflex
#define icanflex

#define Sender true
#define Reciever false

#if defined(USE_CAN_PRIMARY) && !defined(USE_CAN_DATA)
    #define CAN_PRIMARY_BUS CAN1
    #define CAN_DATA_BUS CAN3  // Default to CAN3 if CAN_PRIMARY_BUS is selected
#elif !defined(USE_CAN_PRIMARY) && defined(USE_CAN_DATA)
    #define CAN_PRIMARY_BUS CAN3
    #define CAN_DATA_BUS CAN3
#else
    #error "Please define either USE_CAN_PRIMARY or USE_CAN_DATA"
#endif



class iCANflex{
    public:

    iCANflex();
    FlexCAN_T4<CAN_PRIMARY_BUS, RX_SIZE_256, TX_SIZE_16> can_primary;
    FlexCAN_T4FD<CAN_DATA_BUS, RX_SIZE_256, TX_SIZE_16> can_data;
    CAN_message_t msg;
    CANFD_message_t msgFD;


   
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

    bool begin();
    int readData(CAN_message_t t);
    void rawData(INT32U*, INT8U*);
    void send(long , byte[]);
};







#endif 
