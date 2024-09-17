#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPS++.h>
#include <MPU9250_WE.h>
#include "ALIVE_defs.h"
#include "packets.h"

#define MPU9250_ADDR 0x68
#define GPSBaudRate  9600

void start_module_device(void);
void acq_function(int acq_mode);

/* Accelerometer && GPS functions */
void imu_acq_function(void);
void gps_acq_function(void);

/* CAN Acquisition functions */
void cleanDTC(void);

/* Packet Message Functions */
BLE_packet_t updatePacket(void);

#endif
