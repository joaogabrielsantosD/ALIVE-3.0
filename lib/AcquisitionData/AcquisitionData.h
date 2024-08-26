#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPSPlus.h>
#include <MPU9250_WE.h>
#include <pthread.h>
#include "CircularBufferState.h"
#include "packets.h"
#include "Messages.hpp"

#define MPU9250_ADDR 0x68

typedef void *ThreadHandle_t;

void start_module_device(void);
void acq_function(int acq_mode);

/* Accelerometer functions */
bool initializeMPU9250(void);
void imu_acq_function(void);

/* CAN Acquisition functions */
void Handling_CAN_msg(void);
void cleanDTC(void);

/* Packet Message Functions */
BLE_packet_t updatePacket(void);

#endif
