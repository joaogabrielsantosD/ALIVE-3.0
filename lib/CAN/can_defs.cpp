#include "can_defs.h"

#ifdef CAN_2515
    mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
    bool receive_message = false;
#endif
bool _ext = false;

bool start_CAN_device(bool set_filt)
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
        if(CAN.begin(CAN_500KBPS, MCP_8MHz)==CAN_OK)
        {
            Serial.println("CAN init ok!!!");
            init_flag = true; // Marks the flag that indicates correct CAN initialization
            break; // get out of the loop
        }
        init_flag = false; // Mark the flag that indicates there was a problem initializing the CAN
    }

    if(init_flag)
    {
        if(set_filt) set_mask_filt();
        attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canISR, FALLING);
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

void SaveParameters_extended(bool ext)
{
    _ext = ext;
}

bool send_msg(unsigned char *msg)
{
    return send_msg(msg, _ext);
}

bool send_msg(unsigned char *msg, bool extended)
{
    return CAN.sendMsgBuf(CAN_ID(extended), extended, 8, msg)==CAN_OK ? true : false;
}

uint32_t get_ID_mode()
{
    return CAN.getCanId();
}

bool msg_receive()
{
    return CAN.checkReceive()==CAN_MSGAVAIL;
}

void get_msg(unsigned char messageData[], uint32_t &id, uint8_t &len)
{
    receive_message = false;
    // Reads message and ID
    id = CAN.getCanId();  
    CAN.readMsgBuf(&len, messageData); 
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
