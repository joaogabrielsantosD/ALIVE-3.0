#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPSPlus.h>
#include <MPU9250.h>
#include <pthread.h>
#include "StateMachine.h"
#include "packets.h"

typedef void *ThreadHandle_t;

void IMU_GPS_Data_Acquisition(void);

/* Accelerometer && GPS functions */
ThreadHandle_t imu_acq_function(void);
ThreadHandle_t gps_acq_function(void);

/* CAN Acquisition functions */
void MsgRec_Treatment(void);

/* Packet Message Functions */
BLE_packet_t defaultValue(void);
BLE_packet_t updatePacket(void);

#endif