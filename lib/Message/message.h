#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include "CAN_PIDs.h"

typedef struct 
{
    float teste;
} BLEmsg_t;

void TratamentoDaMsg(unsigned char* info_can, int length);
//void CheckMsg(unsigned char* data);

#endif