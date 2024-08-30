#include "CircularBufferState.h"

/* Variables for Circular Buffer*/
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_id = IDLE_ST;
boolean imu_flag = false, gps_flag = false;
uint8_t PID_enable_bit[16] = {0};
uint8_t PID_Enables_bin[128] = {0};
uint8_t odometer_pid_enable = 0;

int CircularBuffer_state()
{
  bool buffer_full = false;

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

  return current_id;
}

bool insert(int ST)
{   
  if (ST == Accelerometer_ST)
    return imu_flag ? state_buffer.push(ST) : 0;

  else if (ST == GPS_ST)
    return state_buffer.push(ST);    // Try to connect any time, if is possible

  else if (ST == DTC_mode_3)
    return state_buffer.unshift(ST); // marks the DTC as priority in the buffer, placing it first

  else if (ST == Odometer_PID)
    return Verify_odometer_exist() ? state_buffer.push(ST) : 0;

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

void save_flag_gps_parameter(boolean _flag_)
{
  gps_flag = _flag_;
}

String verify_message_is_null(int id, double msg)
{
  switch (id)
  {
    case GPS_ST:
      return gps_flag ? String(msg) : "null";
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
