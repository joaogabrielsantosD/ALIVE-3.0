#ifndef CAN_DEFS_H
#define CAN_DEFS_H

#include <Arduino.h>
#include <SPI.h>
#include "CAN_PIDs.h"
#include "message.h"

bool can_setup(void);
void set_mask_filt(void); 
bool send_msg(unsigned char* msg);
bool trataMsgRecCAN(void);
bool checkReceive(void);
void canISR(void);

#endif