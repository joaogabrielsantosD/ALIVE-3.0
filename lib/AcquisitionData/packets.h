#ifndef PACKETS_H
#define PACKETS_H

#include <sys/_stdint.h>

typedef struct
{
    float acc_x, acc_y, acc_z;
} imu_acc_t;

typedef struct
{
    double LAT, LNG;
} gps_data_t;

typedef struct 
{
    float Calculated_Engine_Load,

        Engine_Coolant_Temperature,
    
        Fuel_Pressure,
    
        Intake_Manifold_Absolute_Pressure_MAP,
    
        Engine_RPM,    
    
        Speed,
    
        Throttle_Position,
    
        Run_Time,
    
        Distance_traveled_with_MIL_on,
    
        Fuel_Level_input,
    
        Distance_traveled_since_codes_cleared,
    
        Ambient_Air_Temperature,
    
        Engine_Oil_Temperature,
    
        Engine_fuel_rate,
    
        Odometer;

    imu_acc_t imu_acc;

    gps_data_t gps_data;

    String DTC;

} BLE_packet_t;

#endif
