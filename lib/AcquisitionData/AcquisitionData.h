#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <Arduino.h>
#include "StateMachine.h"
#include "packets.h"

/* Accelerometer && GPS functions */
// acc function
// gps function

/* CAN Acquisition functions */
void MsgRec_Treatment(void);

/* Packet Message Functions */
BLE_packet_t defaultValue(void);
BLE_packet_t updatePacket(void);

#endif