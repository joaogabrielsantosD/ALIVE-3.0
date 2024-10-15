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

void start_CAN_device(void);
void set_mask_filt(void);
void canISR(void);
uint8_t TestIF_StdExt(void);
bool checkPID(void);
bool send_msg(unsigned char *msg, bool extended);
void Read_CANmsgBuf(unsigned char *Data_can);
void Storage_PIDenable_bit(unsigned char *bit_data, int position);
void debug_print(unsigned char *message, bool response);

void SaveParameters_extended(bool ext);
uint32_t get_CAN_ID(void);

void get_msg(unsigned char messageData[], uint32_t &id, uint8_t &len);

void Handling_PID(uint8_t *PID);
void Convert_Dec2Bin(void);
int Check_bin_for_state(int pid_order);
int Verify_odometer_exist(void);

void send_OBDmsg(int PID);

BLE_packet_t updatePacket();
#endif