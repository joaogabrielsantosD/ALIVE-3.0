#include "CanFunctions.h"

#ifdef CAN_2515
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif

// #define Print_Msg_PIDSuported
#define debug_when_receive_byte
// #define Print_Sended_Msg

CAN_Messages CAN_msg;

BLE_packet_t packet;
uint8_t PID_enable_bit[16] = {0};
uint8_t PID_Enables_bin[128] = {0};
uint8_t odometer_pid_enable = 0;
volatile bool receive_message = false; // flag to enable or disable the message treatment
const unsigned char Pids[] = {PIDs1, PIDs2, PIDs3, PIDs4, PIDs5};
bool _ext = false;

/* Init CAN MCP2515 */
void start_CAN_device()
{
  unsigned long tcanStart = millis();
  const unsigned long cantimeOut = 1000; // (1 second)
  // wait for the CAN shield to initialize
  Serial.println("Connecting CAN...");
  while ((millis() - tcanStart) < cantimeOut) // wait timeout
  {
    if (CAN.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK)
    { // Serial.println("CAN init ok!!!");
      set_mask_filt();
      attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canISR, FALLING);
      pinMode(CAN_DEBUG_LED, OUTPUT);
    }
    else
    {
      Serial.println("MCP2515 error!!!");
      // digitalWrite(CAN_DEBUG_LED, 1);
    }
  }
}

/*Set Filter and masks to receive only OBD2 Messages*/
void set_mask_filt()
{
  // set mask and filter to STANDART Can frame receive [0x7E8 - 0x7EF]
  CAN.init_Mask(0, 0, 0x7E0);
  CAN.init_Filt(0, 0, 0x7E8);

  // set mask and filter to EXTENDED Can frame receive [0x18DAF100 - 0x18DAF1FF]
  CAN.init_Mask(1, 1, 0x18DAF100);
  CAN.init_Filt(1, 1, 0x18DAF100);

  // CAN.init_Mask(1, 1, 0xFFFFFFFF);
  // CAN.init_Filt(1, 1, 0x18DAF100);
}

/* CAN interrupt Callback*/
void canISR()
{
  digitalWrite(CAN_DEBUG_LED, digitalRead(CAN_DEBUG_LED) ^ 1); // Blink Can Led
  receive_message = true;                                      // Flag that indicates that a message was received via CAN
}

/*TODO: Maybe delete this later*/
bool checkReceive()
{
  return receive_message;
}

/*Return CAN ID type, Stardart(0) or Extended (1)*/
bool TestIF_StdExt()
{
  bool extended = true;
  bool TestIDonce = false;
  unsigned char MsgRequest[8] = {0x04, 0x01, 0x00 /*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  unsigned long obd_tstart = millis(), ext_tstart = millis();
  const unsigned long OBD_timout = 3000; // 3 seconds

  // Serial.println("Testing can ID type...");

  while (CAN.checkReceive() == CAN_NOMSG && TestIDonce == false)
  {
    if ((millis() - ext_tstart) <= 200)
    {
      extended = false;
      send_msg(MsgRequest, extended);
      Serial.println("Testing Standart...");
    }
    else
    {
      extended = true;
      Serial.println("Testing Extended...");
      send_msg(MsgRequest, extended);
      if ((millis() - ext_tstart) >= 400)
      {
        ext_tstart = millis();
      }
    }
    vTaskDelay(100);
    TestIDonce = true;

    if ((millis() - obd_tstart) >= OBD_timout)
      Serial.println("Trying to connect with CAN BUS, turn on your vehicle!!!"); // timeout for OBD II connection failed
  }

  return extended;
}

/*Request which PID's are available to read */
bool checkPID(bool ext)
{
  bool CheckPIDFail = false;

  unsigned char MsgRequest[] = {0x04, 0x01, 0x00 /*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t Data_can[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  unsigned long obd_tstart = millis();
  const unsigned long OBD_timout2 = 5000; // 5 seconds

  for (int i = 0; i < sizeof(Pids); i++)
  {
#ifdef Print_Msg_PIDSuported
    Serial.printf("Trying to send PID[%d] support, please turn on the car electronics\r\n", i + 1);
    debug_print(MsgRequest, true);
#endif

    MsgRequest[2] = Pids[i];

    while (CAN.checkReceive() == CAN_NOMSG || (CAN.getCanId() != 0x7E8 && CAN.getCanId() != 0x18DAF100))
    {
      send_msg(MsgRequest, ext);
      vTaskDelay(100);
      // Serial.println("Trying to receive correct pid message!!!");

      if ((millis() - obd_tstart) >= OBD_timout2)
        CheckPIDFail = true;
    }

    if (CheckPIDFail)
      memset((Data_can + 3), 0xFF, 5);
    else
      Read_CANmsgBuf(Data_can);

    Storage_PIDenable_bit(Data_can, i * 4);
  }

  for (size_t i = 0; i < sizeof(PID_Enables_bin); i++)
  {
    Serial.print(PID_Enables_bin[i]);
  }
  // Serial.printf("\t%d\r\n", odometer_pid_enable);

  return true;
}

/*Storage Binary Array with vehicle PID's availables*/
void Storage_PIDenable_bit(unsigned char *bit_data, int position)
{
  if (position < sizeof(PID_enable_bit))
    for (int i = 0; i < 4; i++)
      PID_enable_bit[position + i] = bit_data[4 + i];

  // Convert Dec to Bin
  if (position == 12)
  {
    for (int i = 0; i < 16; i++)
    {
      uint8_t Aux = PID_enable_bit[i];
      int k = (i + 1) * 8 - 1;

      for (int j = 0; j < 8; j++)
      { // loop for complete the 8 bits of the uint8_t variable
        PID_Enables_bin[k--] = Aux % 2;
        Aux /= 2;
      }
    }
  }
  else if (position == PID_to_index_5)
    odometer_pid_enable = ((bit_data[4] >> 2) & ~0xFE); // move to 1 and disable the others bit
}

/*Return Binary Array with vehicle PID's availables*/
int Check_bin_for_state(int pid_order)
{
  Serial.printf(" BIN: %d", PID_Enables_bin[pid_order - 1]);
  return PID_Enables_bin[pid_order - 1] & 0x01;
}

/*Send Can message to BUS*/
bool send_msg(unsigned char *msg, bool extended)
{
  return CAN.sendMsgBuf(CAN_ID(extended), extended, 8, msg) == CAN_OK ? true : false;
}

/*Read messageData and ID from Can buffer*/
uint32_t Read_CANmsgBuf(uint8_t *Data_can)
{
  uint8_t length = 8;
  uint32_t ID = 0;

  while (CAN.checkReceive() == CAN_MSGAVAIL)
  {
    receive_message = false;
    ID = CAN.getCanId();
    CAN.readMsgBuf(&length, Data_can);

#ifdef debug_when_receive_byte
    debug_print(Data_can, false);
#endif
  }
  return ID;
}

int Verify_odometer_exist()
{
  return odometer_pid_enable & 0x01;
}

/*If pass: True - Print Send  False - Print Received*/
void debug_print(unsigned char *message, bool response)
{
  Serial.printf("%s CAN: id 0x", response ? "Send to" : "Received by");
  Serial.print(CAN.getCanId(), HEX);
  Serial.print("  ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(*(message + i), HEX);
    Serial.print("\t");
  }
  Serial.println();
}

/*Send CANmsg to get data from vehicle*/
void send_OBDmsg(int PID, bool CANidType)
{
  uint32_t id = 0;
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
  while (CAN.checkReceive() != CAN_MSGAVAIL) // Wait the response
  {
    send_msg(messageData, CANidType); // Send the resquest

#ifdef Print_Sended_Msg
    debug_print(messageData, true);
#endif

    vTaskDelay(100);

    // timeout
    if (millis() - initialTime >= 1000)
      return;
  }

  id = Read_CANmsgBuf(messageData);

  if (id == 0x7E8 || id == 0x18DAF100)
    CAN_msg.Handling_Message(messageData, &packet);
}

/*================================== Packet Message Functions ==================================*/
BLE_packet_t updatePacket()
{
  return packet;
}