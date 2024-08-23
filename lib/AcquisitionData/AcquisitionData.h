#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPSPlus.h>
#include <MPU9250_WE.h>
#include <pthread.h>
#include "StateMachine.h"
#include "packets.h"
#include "Messages.hpp"

typedef void *ThreadHandle_t;

void start_module_device(void);
void acq_function(int acq_mode);
void imu_acq_function();

void initializeMPU9250();
void calibrateMPU9250();


/* CAN Acquisition functions */
void Handling_CAN_msg(void);
void cleanDTC(void);

/* Packet Message Functions */
BLE_packet_t updatePacket(void);

#endif