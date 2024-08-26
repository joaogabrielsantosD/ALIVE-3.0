#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPSPlus.h>
#include <MPU9250.h>
#include <pthread.h>
#include "StateMachine.h"
#include "packets.h"
#include "Messages.hpp"

typedef void *ThreadHandle_t;

void start_module_device(void);
void acq_function(int acq_mode);

/* Accelerometer && GPS functions */
ThreadHandle_t imu_acq_function(void *arg);
ThreadHandle_t gps_acq_function(void *arg);

/* CAN Acquisition functions */
void Handling_CAN_msg(void);
void cleanDTC(void);

/* Packet Message Functions */
BLE_packet_t updatePacket(void);

#endif