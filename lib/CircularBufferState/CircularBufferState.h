#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#pragma once

#include <CircularBuffer.hpp>
#include "ALIVE_defs.h"
#include "CanFunctions.h"

#define BUFFER_SIZE 200

class CircularBufferStateHandler
{
private: /* Variables for Circular Buffer */
    CircularBuffer<int, BUFFER_SIZE> state_buffer;
    int current_pid = IDLE_ST;

public:
    CircularBufferStateHandler() = default;

    /**
    * @brief Retrieves the next PID from the circular buffer
    * @note Removes and returns the oldest element from the buffer, storing it in `current_pid`.  
    * If the buffer is empty, assigns IDLE_ST to current_pid.  
    * @note Prints the buffer contents if `CIRCULAR_BUFFER_DEBUG` is enabled.  
    * @return 
    *   - The retrieved PID if the buffer is not empty
    *   - IDLE_ST if the buffer is empty
    */
    int CircularBuffer_state(void);

    CircularBufferStateHandler &operator>>(int& st) {
        st = CircularBuffer_state();
        return *this;
    } 

    /**
    * @brief Inserts a PID into the circular buffer  
    * @note Adds the given PID to the buffer based on its priority and type.  
    * - `DTC_mode_3` is prioritized and placed at the front.  
    * - `Odometer_PID` is inserted only if it exists.  
    * - `Other PIDs` are inserted at the front (if slow_msg) or at the back (if not slow_msg).  
    * @param ST The PID to be inserted.  
    * @param slow_msg Defines whether the message is slow (higher priority). Default is true.  
    * @return 
    *   - The result of the insertion
    *   - -1 if the PID is not valid.  
    */
    int insert(int ST, bool slow_msg = true);

    /**
        @note DEPRECATED
        @brief This function is used to verify if the message is null
        @param id: the id of the message
        @param msg: the message to be verified
        @return: the message if it is not null
    */
    String verify_message_is_null(int id, double msg); // <------ DEPRECATED

private:
    /**
    * @brief Debug the CircularBuffer 
    * @note Prints the contents of the circular buffer in hexadecimal format,
    * showing its current size, capacity, and whether it is full or empty.
    */
   void printBuffer(void);
};

extern CircularBufferStateHandler CircularBufferState;

#endif // STATEMACHINE_H
