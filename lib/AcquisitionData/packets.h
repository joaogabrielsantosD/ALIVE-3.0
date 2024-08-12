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
    float Bank1, Bank2;
} ShortTermFuel_t;

typedef struct
{
    float Bank1, Bank2;
} LongTermFuel_t;

typedef struct 
{
    float _Bank1_Sensor1, _Bank1_Sensor2, _Bank1_Sensor3, _Bank1_Sensor4,
    _Bank2_Sensor1, _Bank2_Sensor2, _Bank2_Sensor3, _Bank2_Sensor4;

    float bk1_ss1, bk1_ss2, bk1_ss3, bk1_ss4,
    bk2_ss1, bk2_ss2, bk2_ss3, bk2_ss4;
} OxygenSensorVolt_Short_Term_FuelTrim_t;

typedef struct
{
    float Calculated_Engine_Load,

        Engine_Coolant_Temperature,

        Fuel_Pressure,

        Intake_Manifold__MAP,

        Engine_RPM,

        Speed,

        Throttle_Position,

        Run_Time,

        Distance_traveled_with_MIL_on,

        Fuel_Level_input,

        Distance_traveled,

        Ambient_Air_Temperature,

        Engine_Oil_Temperature,

        Engine_fuel_rate,

        Timing_Advance,

        Intake_Air_Temperature,

        MAF_Air_FlowRate,

        Odometer;
    
    union
    {
        ShortTermFuel_t ShortTermFuel;
        LongTermFuel_t LongTermFuel; 
        OxygenSensorVolt_Short_Term_FuelTrim_t OxygenSensorVolt; 
    } Short_Long_Term;

    imu_acc_t imu_acc;

    gps_data_t gps_data;

    String DTC;

} BLE_packet_t;

#endif
