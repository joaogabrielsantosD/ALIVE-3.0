#ifndef PACKETS_H
#define PACKETS_H

#include <sys/_stdint.h>

typedef struct {
    uint8_t Calculated_Engine_Load;

    uint8_t Engine_Coolant_Temperature;

    uint16_t Fuel_Pressure;

    uint8_t Intake_Manifold_Absolute_Pressure_MAP;

    uint16_t Engine_RPM;

    uint8_t Speed;

    uint8_t Throttle_Position;

    uint16_t Run_Time;

    uint16_t Distance_traveled_with_MIL_on;

    uint8_t Fuel_Level_input;

    uint16_t Distance_traveled_since_codes_cleared;

    int16_t Ambient_Air_Temperature;

    int16_t Engine_Oil_Temperature;

    float Engine_fuel_rate;
    
    float Odometer;

} BLE_packet_t;


#endif