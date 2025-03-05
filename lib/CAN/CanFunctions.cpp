#include "CanFunctions.h"

// #define Print_Msg_PIDSuported
// #define debug_when_receive_byte
// #define Print_Sended_Msg

CANHandler CAN;
static bool receive_message = false;

bool CANHandler::start_CAN_device()
{
  unsigned long tcanStart = millis();
  const unsigned long cantimeOut = 1000; // (1 second)
  // wait for the CAN shield to initialize
  Serial.println("Connecting CAN...");
  while ((millis() - tcanStart) < cantimeOut) // wait timeout
  {
    if (canShield.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK)
    {
      // Serial.println("CAN init ok!!!");
      this->set_mask_filt();
      attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canISR, FALLING);
      pinMode(CAN_DEBUG_LED, OUTPUT);
      return true;
    }

    else
    {
      Serial.println("MCP2515 error!!!");
      // digitalWrite(CAN_DEBUG_LED, 1);
      return false;
    }
  }
  return false;
}

void CANHandler::set_mask_filt()
{
  // set mask, set both the mask to 0x3ff
  canShield.init_Mask(0, 1, 0x1FFFFFFF);
  canShield.init_Mask(1, 1, 0x1FFFFFFF);

  // set filter, we can receive id from 0x04 ~ 0x09
  for (int i = 0; i < 6; i++)
    canShield.init_Filt(i, 1, 0x18DAF110);
}

void CANHandler::canISR()
{
  digitalWrite(CAN_DEBUG_LED, digitalRead(CAN_DEBUG_LED) ^ 1); // Blink Can Led
  receive_message = true; // Flag that indicates that a message was received via CAN
}

uint8_t CANHandler::TestIF_StdExt()
{
  bool extended = true;
  unsigned char MsgRequest[8] = {0x02, 0x01, 0x00 /*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};
  unsigned long obd_tstart = millis(), ext_tstart = millis();
  const unsigned long OBD_timout = 3000; // 3 seconds

  while (canShield.checkReceive() == CAN_NOMSG && !receive_message)
  {
    if ((millis() - ext_tstart) <= 200)
    {
      extended = false;
      this->send_msg(MsgRequest, extended);
      #ifdef Print_Msg_PIDSuported
        Serial.println("Testing Standart...");
      #endif
    }

    else
    {
      extended = true;
      #ifdef Print_Msg_PIDSuported
        Serial.println("Testing Extended...");
      #endif
      this->send_msg(MsgRequest, extended);

      if ((millis() - ext_tstart) >= 400)
        ext_tstart = millis();
    }

    vTaskDelay(100);

    if ((millis() - obd_tstart) >= OBD_timout)
    {
      #ifdef Print_Msg_PIDSuported
        Serial.println("Trying to connect with CAN BUS, turn on your vehicle!!!"); // timeout for OBD II connection failed
      #endif
    }
  }

  this->_ext = extended;
  return (uint8_t)extended;
}

bool CANHandler::checkPID()
{
  unsigned char MsgRequest[] = {0x04, 0x01, 0x00 /*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t Data_can[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  for (auto i = 0; i < sizeof(Pids); i++)
  {
    #ifdef Print_Msg_PIDSuported
      Serial.printf("Trying to send PID[%d] support, please turn on the car electronics\r\n", i + 1);
      this->debug_print(MsgRequest, true);
    #endif

    MsgRequest[2] = this->Pids[i];

    while (canShield.checkReceive() == CAN_NOMSG)
    {
      this->send_msg(MsgRequest, _ext);
      vTaskDelay(400);
    }

    this->Read_CANmsgBuf(Data_can);

    this->Storage_PIDenable_bit(Data_can, i * 4);
  }

  return true;
}

void CANHandler::Storage_PIDenable_bit(unsigned char *bit_data, int position)
{
  if (position < sizeof(PID_enable_bit))
  {
    if (*(bit_data + 2) == Pids[position / 4])
      for (int i = 0; i < 4; i++)
        this->PID_enable_bit[position + i] = bit_data[4 + i - 1];

    else if (*(bit_data + 3) == Pids[position / 4])
      for (int i = 0; i < 4; i++)
        this->PID_enable_bit[position + i] = bit_data[4 + i];
  }

  // Convert Dec to Bin
  if (position == 12)
  {
    for (int i = 0; i < 16; i++)
    {
      uint8_t Aux = this->PID_enable_bit[i];
      int k = (i + 1) * 8 - 1;

      for (int j = 0; j < 8; j++)
      { // loop for complete the 8 bits of the uint8_t variable
        this->PID_Enables_bin[k--] = Aux % 2;
        Aux /= 2;
      }
    }
  }
  
  else if (position == PID_to_index_5)
    this->odometer_pid_enable = ((*(bit_data + 4) >> 2) & ~0xFE); // move to 1 and disable the others bit
}

void CANHandler::Read_CANmsgBuf(uint8_t *Data_can)
{
  uint8_t length = 8;
  uint32_t ID = 0;

  while (canShield.checkReceive() == CAN_MSGAVAIL)
  {
    canShield.readMsgBuf(&length, Data_can);
    ID = canShield.getCanId();

    #ifdef debug_when_receive_byte
      this->debug_print(Data_can, false);
    #endif
  }
}

int CANHandler::Check_bin_for_state(int pid_order)
{
  return PID_Enables_bin[pid_order - 1] & 0x01;
}

int CANHandler::Verify_odometer_exist()
{
  return odometer_pid_enable & 0x01;
}

void CANHandler::send_OBDmsg(int PID, BLE_packet_t *packet)
{
  unsigned long initialTime = 0;
  unsigned char messageData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  /*{0x00, 0x00, PID, 0x00, 0x00, 0x00, 0x00, 0x00}*/

  if (PID != DTC_mode_3)
  {
    messageData[0] = 0x02;               // Lenght
    messageData[1] = 0x01;               // Mode = Current Data
    messageData[2] = (unsigned char)PID; // PID
  }

  else
  {
    messageData[0] = 0x01;
    messageData[1] = 0x03; // Mode = Stored DTC
    messageData[2] = 0x00;
  }

  initialTime = millis();
  while (canShield.checkReceive() == CAN_NOMSG) // Wait the response
  {
    this->send_msg(messageData, _ext); // Send the resquest

    #ifdef Print_Sended_Msg
      this->debug_print(messageData, true);
    #endif

    vTaskDelay(100);

    // timeout
    if (millis() - initialTime >= 500)
      return;
  }

  this->Read_CANmsgBuf(messageData);

  if (PID != DTC_mode_3)
    CAN_msg.Handling_Message(messageData, packet);
  else
    CAN_msg.Read_DTC(messageData, packet);
}

bool CANHandler::send_msg(unsigned char *msg, bool extended)
{
  return canShield.sendMsgBuf(CAN_ID(extended), extended, 8, msg) == CAN_OK;
}

void CANHandler::debug_print(unsigned char *message, bool response)
{
  Serial.printf("%s CAN: id 0x", response ? "Send to" : "Received by");
  Serial.print(canShield.getCanId(), HEX);
  Serial.print("  ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(*(message + i), HEX);
    Serial.print("\t");
  }
  Serial.println();
}
