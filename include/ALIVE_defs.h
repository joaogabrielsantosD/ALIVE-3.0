#ifndef ALIVE_DEFS_H
#define ALIVE_DEFS_H

#include <driver/gpio.h>
#include <HardwareSerial.h>
#include "CAN_PIDs.h"

/*====================== CAN DEFINITIONS ============================ */
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

#define PID_to_index_5      16

#endif
