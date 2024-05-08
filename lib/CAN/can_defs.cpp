#include "can_defs.h"

#ifdef CAN_2515
    //#include "mcp2515_can.h"
    mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
    bool receive_message = false;
    //#define MAX_DATA_SIZE 8
#endif

bool can_setup()
{
    /* Start the can */
    bool init_flag = false;
    unsigned long tcanStart = 0, cantimeOut = 0;
    tcanStart = millis();
    cantimeOut = 1000; // (1 second)
    // wait for the CAN shield to initialize
    Serial.println("Connecting CAN...");
    while((millis() - tcanStart) < cantimeOut) // wait timeout
    { 
        if(CAN.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK)
        {
            Serial.println("CAN init ok!!!");
            init_flag = true; // Marks the flag that indicates correct CAN initialization
            break; // get out of the loop
        }
        init_flag = false; // Mark the flag that indicates there was a problem initializing the CAN
    }

    return init_flag;
}

void set_mask_filt() 
{
  // set mask, set both the mask to 0x3ff
  CAN.init_Mask(0, 1, 0x1FFFFFFF);
  CAN.init_Mask(1, 1, 0x1FFFFFFF);

  // set filter, we can receive id from 0x04 ~ 0x09
  CAN.init_Filt(0, 1, 0x18DAF110);
  CAN.init_Filt(1, 1, 0x18DAF110);

  CAN.init_Filt(2, 1, 0x18DAF110);
  CAN.init_Filt(3, 1, 0x18DAF110);
  CAN.init_Filt(4, 1, 0x18DAF110);
  CAN.init_Filt(5, 1, 0x18DAF110);
}

bool send_msg(unsigned char* msg)
{
    return CAN.sendMsgBuf(CAN_ID, 1, 8, msg)==CAN_OK ? true : false;
}

bool MsgRec_CANroutine()
{
    receive_message = false;
    while(CAN.checkReceive()==CAN_MSGAVAIL)
    {
        unsigned char messageData[8];
        uint32_t messageId;
        unsigned char len = 0; // Stores the size of the CAN message (quantity of data received)

        // Reads message and ID
        CAN.readMsgBuf(&len, messageData); 
        messageId = CAN.getCanId();        
    }
    
    return receive_message;
}

bool checkReceive()
{
    return receive_message;
}

/* CAN interrupt */
void canISR()
{
   receive_message = true; // Flag that indicates that a message was received via CAN
}
