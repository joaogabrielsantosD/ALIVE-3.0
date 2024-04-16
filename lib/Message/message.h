#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include "CAN_PIDs.h"

typedef struct 
{
    float EngineCollant, Engine_rpm, vehicle_speed, fuellevel, Distance_travel;
} BLEmsg_t;

void TratamentoDaMsg(unsigned char* info_can, int length);
BLEmsg_t defaultmsg();
BLEmsg_t requestMsg();

#endif