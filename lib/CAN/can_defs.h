#ifndef CAN_DEFS_H
#define CAN_DEFS_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515_can.h>
#include "ALIVE_defs.h"

bool start_CAN_device(bool set_filt = false);
void set_mask_filt(void); 
bool send_msg(unsigned char* msg);
bool msg_receive(void);
void get_msg(unsigned char messageData[], uint32_t& id, uint8_t& len);
bool checkReceive(void);
void canISR(void);

#endif