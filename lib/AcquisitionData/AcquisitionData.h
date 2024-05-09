#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <Arduino.h>
#include <CircularBuffer.hpp>
#include "can_defs.h"

typedef struct 
{
    float a;
} wifi_packet_t;

/* Circular Buffer Functions */
int CircularBuffer_state(void);
bool insert(int ST);
void debug_print(unsigned char* message);

/* Accelerometer && GPS functions */
// acc function
// gps function

/* CAN Acquisition functions */
void MsgRec_Treatment(void);
void Storage_PIDenable_bit(unsigned char* bit_data, int8_t position);
void Convert_Dec2Bin(void);
int Check_bin_for_state(int pid_order);
int Verify_odometer_exist(void);

#endif