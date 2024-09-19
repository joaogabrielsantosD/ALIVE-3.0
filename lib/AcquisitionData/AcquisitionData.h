#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <TinyGPS++.h>
#include <MPU9250_WE.h>
#include "Definitions/ALIVE_defs.h"
#include "CollectedData/CollectedDataStruct.h"

#define MPU9250_ADDR 0x68
#define GPSBaudRate  9600

void start_module_device(void);

/* Accelerometer && GPS functions */
void imu_acq_function(void);
void gps_acq_function(void);

/* CAN Acquisition functions */
void cleanDTC(void);



#endif
