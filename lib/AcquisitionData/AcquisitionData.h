#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <Arduino.h>
#include "StateMachine.h"

typedef struct {
    float a;
} wifi_packet_t;

/* Accelerometer && GPS functions */
// acc function
// gps function

/* CAN Acquisition functions */
void MsgRec_Treatment(void);

#endif