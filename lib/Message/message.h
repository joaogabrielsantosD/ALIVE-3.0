#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include "CAN_PIDs.h"

#define PID_to_index_1 (0)
#define PID_to_index_2 (1<<2)
#define PID_to_index_3 (1<<3)
#define PID_to_index_4 (3<<2)

typedef struct 
{
    float EngineCollant, Engine_rpm, vehicle_speed, fuellevel, Distance_travel;
} BLEmsg_t;
void Convert_Dec2Bin(uint8_t* PID_Enables);
void MsgRec_Treatment(unsigned char* info_can, int length);
void Storage_PIDenable_bit(unsigned char* bit_data, int8_t position);
bool Check_bin_for_state(int pid_order);
BLEmsg_t defaultmsg();
BLEmsg_t requestMsg();

#endif