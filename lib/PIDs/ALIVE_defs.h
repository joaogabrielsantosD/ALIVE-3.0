#ifndef ALIVE_DEFS_H
#define ALIVE_DEFS_H

#include <driver/gpio.h>
#include <HardwareSerial.h>
#include "CAN_PIDs.h"

/*====================== CAN DEFINITIONS ============================ */
#define SerialAT            Serial2
#define CAN_2515
#define BUFFER_SIZE         50
#define CAN_ID_EXTENDED     0x18DB33F1
#define CAN_ID_NORMAL       0x7DF
#define CAN_ID(EXT)         ((EXT) ? (CAN_ID_EXTENDED) : (CAN_ID_NORMAL))      

#define PIDs1               PIDsupported1
#define PIDs2               PIDsupported2
#define PIDs3               PIDsupported3
#define PIDs4               PIDsupported4
#define PIDs5               PIDsupport5
#define FueL_Status_PID     FuelSystem
#define Fuel_Pressure_PID   FuelPressure
#define Fuel_Level_PID      FuelLevelInput
#define Speed_PID           VehicleSpeed
#define Engine_LoadP_ID     EngineLoad
#define Engine_CoolantP_ID  EngineCollantTemp
#define Engine_RPM_ID       EngineRPM
#define Run_Time_PID        RunTime
#define Engine_Oil_PID      EngineOilTemperature
#define Engine_FuelRate_PID EngineFuelRate
#define Ambient_Temp_PID    AmbientAirTemperature
#define Throttle_Pos_PID    ThrottlePosition
#define Distance_on_MIL_PID DistanceTraveled
#define Distance_Travel_PID DistanceTraveledSinceCodeCleared
#define MAP_sensor_PID      AbsolutePressure
#define Odometer_PID        0xA6

#define IDLE_ST             0x00
#define Accelerometer_ST    0x01
#define GPS_ST              0x02

#define PID_to_index_1      0   
#define PID_to_index_2      4
#define PID_to_index_3      8
#define PID_to_index_4      12
#define PID_to_index_5      16

/*====================== HARDWARE DEFINITIONS ============================ */
#define CAN_DEBUG_LED GPIO_NUM_25
#define BLE_DEBUG_LED GPIO_NUM_26
#define SPI_CS_PIN    GPIO_NUM_4  // Pin CS to the MCP2515 module
#define CAN_INT_PIN   GPIO_NUM_27 // Pin used to generate the interrupt by the MCP2515 module

#endif