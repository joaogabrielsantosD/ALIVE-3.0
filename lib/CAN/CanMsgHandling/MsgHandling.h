#ifndef MESSAGES_H
#define MESSAGES_H

#pragma once

#include <Arduino.h>
#include "CollectedDataStruct.h"
#include "CAN_PIDs.h"
#include "ALIVE_defs.h"

/**
 * @brief Converts a numeric value to a hexadecimal character  
 * @note Maps values 10-15 to their corresponding hexadecimal characters ('A'-'F').  
 * If the input value is below 10, it returns 'Z' as an error flag.  
 * @param c The numeric value to convert.  
 * @return The corresponding hexadecimal character or 'Z' for invalid input.  
 */
#define verify_char(c) (c == 10 ? 'A' : c == 11 ? 'B' \
                                    : c == 12   ? 'C' \
                                    : c == 13   ? 'D' \
                                    : c == 14   ? 'E' \
                                    : c == 15   ? 'F' \
                                                : 'Z')
/**
 * @brief Debug levels for CAN_Messages class  
 * @note Defines different levels of debug output for serial printing:  
 * - 0: No serial output  
 * - 1: Print only CAN messages  
 * - 2: Print only DTC messages  
 * - 4: Print both DTC and CAN messages  
 */
#define debug_message 4

class CAN_MessagesHandler
{
private:

    /**
    * @brief Generates a DTC (Diagnostic Trouble Code) string  
    * @note Converts two 8-bit message values into a standardized DTC code.  
    * @param first_msg The first byte containing the DTC type and high bits.  
    * @param second_msg The second byte containing the low bits of the DTC code.  
    * @return A formatted DTC code as a string.  
    */
    String make_DTC_code(uint8_t first_msg, uint8_t second_msg);

    /**
    * @brief Determines the fuel type based on its identifier  
    * @note Maps a given fuel type code to its corresponding fuel type description.  
    * @param type The fuel type identifier (OBD-II standard).  
    * @return A string representing the fuel type.  
    */
    String Check_type_of_fuel(uint8_t type);

public:
    CAN_MessagesHandler() = default;

    /**
    * @brief Reads and processes Diagnostic Trouble Codes (DTC).  
    * @note Extracts DTC data from the provided PID and stores it in the BLE packet.  
    * This function is used to retrieve error codes from the vehicle's ECU.  
    * @param PID Pointer to the PID data containing DTC information.  
    * @param packet Pointer to the BLE packet structure where the DTC data will be stored.  
    */
    void Read_DTC(uint8_t *PID, BLE_packet_t *packet);

    /**
    * @brief Handles and processes a received CAN message.  
    * @note Parses the PID data and stores relevant information in the BLE packet.  
    * This function is responsible for interpreting and formatting vehicle data.  
    * @param PID Pointer to the PID data from the CAN message.  
    * @param packet Pointer to the BLE packet structure where the processed data will be stored.  
    */
    void Handling_Message(uint8_t *PID, BLE_packet_t *packet);
};

#endif
