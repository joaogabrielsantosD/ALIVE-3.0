#ifndef ALIVE_DEFS_H
#define ALIVE_DEFS_H

#include <driver/gpio.h>
#include <HardwareSerial.h>
#include "CAN_PIDs.h"

/*====================== CAN DEFINITIONS ============================ */
#define SerialGPS           Serial2
#define CAN_2515
#define BUFFER_SIZE         200
#define CAN_ID_EXTENDED     0x18DB33F1
#define CAN_ID_NORMAL       0x7DF
#define CAN_ID(EXT)         ((EXT) ? (CAN_ID_EXTENDED) : (CAN_ID_NORMAL))      

#define PIDs1               PIDsupported1
#define PIDs2               PIDsupported2
#define PIDs3               PIDsupported3
#define PIDs4               PIDsupported4
#define PIDs5               PIDsupport5
#define Odometer_PID        0xA6

#define Save_PIDs_Enable    0xAAAA
#define DTC_mode_3          0xFFFF // 2 bytes of PID

#define IDLE_ST             0x00
#define Accelerometer_ST    0x01
#define GPS_ST              0x02

#define PID_to_index_1      0   
#define PID_to_index_2      4
#define PID_to_index_3      8
#define PID_to_index_4      12
#define PID_to_index_5      16

/*====================== HARDWARE DEFINITIONS ============================ */
#define CAN_DEBUG_LED GPIO_NUM_25 // Pin to debug led of CAN communication 
#define BLE_DEBUG_LED GPIO_NUM_26 // Pin to debug led of BLE communication 
#define SPI_CS_PIN    GPIO_NUM_5  // Pin CS to the MCP2515 module
#define CAN_INT_PIN   GPIO_NUM_27 // Pin used to generate the interrupt by the MCP2515 module

#endif
