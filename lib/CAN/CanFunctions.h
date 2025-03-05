#ifndef CANFUNCTIONS_H
#define CANFUNCTIONS_H

#pragma once

#include <mcp2515_can.h>
#include "CanMsgHandling/MsgHandling.h"

#define SPI_CS_PIN    GPIO_NUM_5     // Pin CS to the MCP2515 module
#define CAN_INT_PIN   GPIO_NUM_27   // Pin used to generate the interrupt by the MCP2515 module
#define CAN_DEBUG_LED GPIO_NUM_25 // Pin to debug led of CAN communication

class CANHandler
{
private:
    mcp2515_can canShield = mcp2515_can(SPI_CS_PIN); // Set CS pin
    CAN_MessagesHandler CAN_msg;

    uint8_t PID_enable_bit[16] = {0};
    uint8_t PID_Enables_bin[128] = {0};
    uint8_t odometer_pid_enable = 0x00;
    unsigned char Pids[5] = {PIDs1, PIDs2, PIDs3, PIDs4, PIDs5};
    bool _ext = false; 
    
public:
    CANHandler() = default;

    /**
     * @brief Init CAN MCP2515
     * @note Initialize and start the CAN device.  
     * @return Returns `true` if the device starts successfully, otherwise `false`.  
     */
    bool start_CAN_device(void);

    /**
     * @brief Test if the message is standard or extended.  
     * @note Checks if the message is standard or extended.
     * @return 
     *  - Returns 1 for standard message, 
     *  - 2 for extended message, or 
     *  - 0 if invalid.  
     */
    uint8_t TestIF_StdExt(void);

    /**
     * @brief Check if the PID is valid.
     * @note Request which PID's are available to read.
     * @return
     *  - Returns `true` if the PID is valid,
     *  - otherwise `false`.
     */
    bool checkPID(void);

    /**
    * @brief Send a OBD message to the vehicle.  
    * @note Send a OBD message to the vehicle and store the response in the BLE packet.  
    * @param PID The PID to request from the vehicle.  
    * @param packet Pointer to the BLE packet where the response will be stored.  
    */
    void send_OBDmsg(int PID, BLE_packet_t *packet);

    /**
     * @brief Check if a PID is in the binary state list.  
     * @note Return Binary Array with vehicle PID's availables.
     * @return 
     *  - Returns 1 if the PID exists, 
     *  - otherwise 0.  
     */
    int Check_bin_for_state(int pid_order);

    /**
    * @brief Check if the odometer PID is available. 
    * @note Return Binary flag with vehicle Odometer available 
    * @return 
    *   - Returns 1 if available, 
    *   - otherwise 0.  
    */
    int Verify_odometer_exist(void);

private:
    /**
     * @brief CAN ISR function.  
     * @note Interrupt Service Routine for the CAN bus.  
     */
    static void canISR(void);

    /**
     * @brief Set Filter and masks to receive only OBD2 Messages  
     * @note Configures the mask filter for message processing.  
     */
    void set_mask_filt(void);

    /**
     * @brief Enable a PID bit.
     * @note Storage Binary Array with vehicle PID's availables
     * @param bit_data Pointer to the data bit.
     * @param position Position of the PID.
     */
    void Storage_PIDenable_bit(unsigned char *bit_data, int position);

    /**
     * @brief Send a CAN message to the bus.  
     * @note Sends the provided message to the CAN bus. If `extended` is true, 
     * it sends an extended frame; otherwise, it sends a standard frame.  
     * @param msg Pointer to the CAN message data.  
     * @param extended If true, sends an extended message; if false, sends a standard message.  
     * @return 
     *  - Returns `true` if the message is sent successfully, 
     *  - otherwise `false`.  
    */
    bool send_msg(unsigned char *msg, bool extended);

    /**
     * @brief Read the CAN message buffer.  
     * @note Read messageData and ID from Can buffer.  
     * @param Data_can Pointer to the buffer where the CAN message data will be stored.  
     */
    void Read_CANmsgBuf(unsigned char *Data_can);

    /**
     * @brief Print the CAN message.
     * @note 
     *  - If `response` is `true`: prints the **sent** message.
     *  - If `response` is `false`: prints the **received** message.
     * @param message Pointer to the CAN message data.
     * @param response Boolean flag to indicate message type.
    */
    void debug_print(unsigned char *message, bool response);
};

extern CANHandler CAN;

#endif