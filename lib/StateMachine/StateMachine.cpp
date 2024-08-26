#include "StateMachine.h"

//#define debug_when_send     //Variable to print the messageStructure when sended to Car, Uncomment to Enable.

/* Variables for Circular Buffer*/
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_id = IDLE_ST;
boolean imu_flag = false;
uint8_t PID_enable_bit[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t PID_Enables_bin[128];
uint8_t odometer_pid_enable = 0;

int CircularBuffer_state()
{
  bool buffer_full = false;
  unsigned char messageData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*{0x00, 0x00, PID, 0x00, 0x00, 0x00, 0x00, 0x00}*/

  if (state_buffer.isFull())
  {
    buffer_full = true;
    current_id = state_buffer.pop();
  } else {
    buffer_full = false;
    if (!state_buffer.isEmpty())
      current_id = state_buffer.pop();
    else
      current_id = IDLE_ST;
  }

  switch (current_id)
  {
    case IDLE_ST:
      // Serial.println("i");
      break;

    case Accelerometer_ST:
    case GPS_ST:
      // Serial.println("Modulo ACC \\ GPS");
      break;

    default: /* CAN PID msg */

      if (current_id != DTC_mode_3)
      {
        messageData[0] = 0x02;  //Lenght
        messageData[1] = 0x01;  //Mode = Current Data
        messageData[2] = (unsigned char)current_id; //PID
      }
      
      else
      {
        messageData[0] = 0x01;
        messageData[1] = 0x03;  //Mode = Stored DTC
        messageData[2] = 0x00;
      }

      #ifdef debug_when_send
        if (send_msg(messageData))
          debug_print(messageData);
      #else
        send_msg(messageData);
      #endif

      break;
  }

  return current_id;
}

bool insert(int ST)
{
  if (ST == Odometer_PID)
    return Verify_odometer_exist() ? state_buffer.push(ST) : 0;
  else if (ST == DTC_mode_3)
    return state_buffer.unshift(ST); // marks the DTC as priority in the buffer, placing it first
  else if (ST == Accelerometer_ST || ST == GPS_ST)
    return state_buffer.push(ST);
  else
    return Check_bin_for_state(ST) ? state_buffer.push(ST) : 0;
}

void debug_print(unsigned char *message)
{
  Serial.print("Send to CAN: id 0x");
  Serial.print(get_CAN_ID(), HEX);
  Serial.print("  ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(*(message + i), HEX);
    Serial.print("\t");
  }
  Serial.println();
}

void Storage_PIDenable_bit(unsigned char *bit_data, int8_t position)
{
  if (position < sizeof(PID_enable_bit))
  {
    for (int i = 0; i < 4; i++)
      PID_enable_bit[position + i] = bit_data[4 + i];
  }

  if (position == PID_to_index_4)
    Convert_Dec2Bin();
  else if (position == PID_to_index_5)
    odometer_pid_enable = ((bit_data[4] >> 2) & ~0xFE); // move to 1 and disable the others bit
}

void Convert_Dec2Bin()
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

int Check_bin_for_state(int pid_order)
{
  return PID_Enables_bin[pid_order - 1] & 0x01;
}

int Verify_odometer_exist()
{
  return odometer_pid_enable & 0x01;
}

void save_flag_imu_parameter(boolean _flag)
{
  imu_flag = _flag;
}

String verify_message_is_null(int id, double msg)
{
  switch (id)
  {
    case GPS_ST:
      return String(msg);
      break;
    
    case Accelerometer_ST:
      return imu_flag ? String(msg) : "null";
      break;

    case Odometer_PID:
      return Verify_odometer_exist() ? String(msg) : "null";
      break;

    default:
      return Check_bin_for_state(id) ? String(msg) : "null";
      break;
  }
}


