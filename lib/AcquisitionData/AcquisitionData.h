#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPSPlus.h>
#include <MPU9250.h>
#include <pthread.h>
#include "StateMachine.h"
#include "packets.h"

#define verify_char(c) (c == 10 ? 'A' : c == 11 ? 'B' \
                                      : c == 12 ? 'C' \
                                      : c == 13 ? 'D' \
                                      : c == 14 ? 'E' \
                                      : c == 15 ? 'F' : 'Z') // If c < 10, send the character 'Z' as an error flag

typedef void *ThreadHandle_t;

void start_module_device(void);
void acq_function(int acq_mode);

/* Accelerometer && GPS functions */
ThreadHandle_t imu_acq_function(void *arg);
ThreadHandle_t gps_acq_function(void *arg);

/* CAN Acquisition functions */
void MsgRec_Treatment(void);
String make_DTC_code(uint8_t first_msg, uint8_t second_msg);
void cleanDTC(void);

/* Packet Message Functions */
BLE_packet_t updatePacket(void);

#endif