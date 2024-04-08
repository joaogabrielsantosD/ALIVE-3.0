#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Arduino.h>
#include "CAN_PIDs.h"
#include "can_defs.h"

/* CicularBuffer Defs: */
typedef enum {IDLE_ST, DistanceTraveled_ST, EngineRPM_ST, VehicleSpeed_ST, FuelLevel_ST,EngineCoolant_ST} state_t;

state_t CircularBuffer_state(void);
bool insert(state_t ST);

#endif