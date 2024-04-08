#ifndef TICKERISR_H
#define TICKERISR_H

#include <Arduino.h>
#include <Ticker.h>
#include "StateMachine.h"
#include "CAN_PIDs.h"

void setup_ticker(void);
void PIDs_1hz(void);
void PIDs_10hz(void);
void PIDs_20hz(void);

#endif