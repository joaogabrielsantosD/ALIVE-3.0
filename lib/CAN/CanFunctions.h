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

/* Functions to init the CAN */
void start_CAN_device(void);
void set_mask_filt(void);
/* Test the Type of CAN and the PIDs available */
uint8_t TestIF_StdExt(void);
bool checkPID(void);
void Storage_PIDenable_bit(unsigned char *bit_data, int position);
/* Send and Read the message arrived in the CAN bus */
bool send_msg(unsigned char *msg, bool extended);
void Read_CANmsgBuf(unsigned char *Data_can);
/* CAN Interrupt */
void canISR(void);
/* Print the buffer who was sended or received by CAN */
void debug_print(unsigned char *message, bool response);
/* OBD-II data acquisition, send and wait for a response */
void send_OBDmsg(int PID, BLE_packet_t *packet);
/* Functions to check if the PIDs is available */
int Check_bin_for_state(int pid_order);
int Verify_odometer_exist(void);

#endif