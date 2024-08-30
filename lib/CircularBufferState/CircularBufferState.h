#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <CircularBuffer.hpp>
#include "can_defs.h"

/* Circular Buffer Functions */
int CircularBuffer_state(void);
bool insert(int ST);
void debug_print(unsigned char *message);
/* Bit analyze and storage Functions */
void Storage_PIDenable_bit(unsigned char *bit_data, int8_t position);
void Convert_Dec2Bin(void);
int Check_bin_for_state(int pid_order);
int Verify_odometer_exist(void);
void save_flag_imu_parameter(boolean _flag);
void save_flag_gps_parameter(boolean _flag_);
String verify_message_is_null(int id, double msg);

#endif