#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <CircularBuffer.hpp>
#include "Definitions/ALIVE_defs.h"
#include "CanFunctions.h"

/* Circular Buffer Functions */
int CircularBuffer_state(void);
int insert(int ST, bool slow_msg = true);

/* Debug the CircularBuffer */
void printBuffer();

String verify_message_is_null(int id, double msg);

#endif