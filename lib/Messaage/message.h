#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include "CAN_PIDs.h"

void TratamentoDaMsg(unsigned char* info_can, int length);

typedef struct 
{
    float teste;
} BLEmsg_t;

#endif