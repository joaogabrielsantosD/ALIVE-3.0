#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <CircularBuffer.hpp>
#include "Definitions/ALIVE_defs.h"
#include "CanFunctions.h"

/* Circular Buffer Functions */
int CircularBuffer_state(void);
int insert(int ST);
void debug_print(unsigned char *message);
/* Bit analyze and storage Functions */
void save_flag_imu_parameter(bool _flag);
void save_flag_gps_parameter(bool _flag_);
String verify_message_is_null(int id, double msg);

#endif