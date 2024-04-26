#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Arduino.h>
#include <CircularBuffer.hpp>
#include "can_defs.h"
#include "CAN_PIDs.h"

int CircularBuffer_state(void);
bool insert(int ST);
void debug_print(unsigned char* message);

#endif