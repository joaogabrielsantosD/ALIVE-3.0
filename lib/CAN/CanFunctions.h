#ifndef CANFUNCTIONS_H
#define CANFUNCTIONS_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515_can.h>
#include "Definitions/ALIVE_defs.h"
#include "CanMsgHandling/MsgHandling.hpp"

#define CAN_2515
#define SPI_CS_PIN    GPIO_NUM_5  // Pin CS to the MCP2515 module
#define CAN_INT_PIN   GPIO_NUM_27 // Pin used to generate the interrupt by the MCP2515 module

void start_CAN_device();
void set_mask_filt(void);
bool checkPID(bool ext);
void SaveParameters_extended(bool ext);
bool send_msg(unsigned char *msg, bool extended);
uint32_t Read_CANmsgBuf(uint8_t *Data_can);
bool TestIF_StdExt();
void canISR();
void Storage_PIDenable_bit(unsigned char *bit_data, int position);
int Check_bin_for_state(int pid_order);
int Verify_odometer_exist(void);
void debug_print(unsigned char *message, bool response);
void send_OBDmsg(int PID, bool CANidType);

BLE_packet_t updatePacket();
#endif