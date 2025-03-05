#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#pragma once

#include <TinyGPS++.h>
#include <MPU9250_WE.h>
#include "CollectedDataStruct.h"

#define SerialGPS    Serial2
#define GPSBaudRate  9600
#define MPU9250_ADDR 0x68

class ModulesHandler
{
private:
    TinyGPSPlus NEO_M8N = TinyGPSPlus();
    MPU9250_WE MPU9250  = MPU9250_WE(MPU9250_ADDR);
    bool init_ACC = false;

public:
    ModulesHandler() = default;

    /** 
     * @brief Start the module device
     * @note Init the gps serial communication with GPS module and
     *       Init the I2C communication with MPU9250
    */
    void start_module_device();
    
    /** 
     * @brief Acquisition function for the accelerometer
     * @note Get the accelerometer and gyroscope values
     * @param packet: BLE_packet_t struct
    */
    void imu_acq_function(BLE_packet_t *packet);
    
    /** 
     * @brief Acquisition function for the GPS
     * @note Get the GPS values
     * @param packet: BLE_packet_t struct
    */
    void gps_acq_function(BLE_packet_t *packet);
};

extern ModulesHandler modules;

#endif
