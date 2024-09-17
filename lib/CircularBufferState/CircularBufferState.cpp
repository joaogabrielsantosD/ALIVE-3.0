#include "CircularBufferState.h"

/* Variables for Circular Buffer*/
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_id = IDLE_ST;
bool imu_flag = false, gps_flag = false;

int CircularBuffer_state()
{
  return 0;
  // bool buffer_full = false;

  // if (state_buffer.isFull())
  // {
  //   buffer_full = true;
  //   current_id = state_buffer.pop();
  // } else {
  //   buffer_full = false;
  //   if (!state_buffer.isEmpty())
  //     current_id = state_buffer.pop();
  //   else
  //     current_id = IDLE_ST;
  // }

  // return current_id;
}

bool insert(int ST)
{
  return 0;   
  // if (ST == Accelerometer_ST)
  //   return imu_flag ? state_buffer.push(ST) : 0;

  // else if (ST == GPS_ST)
  //   return state_buffer.push(ST);    // Try to connect any time, if is possible

  // else if (ST == DTC_mode_3)
  //   return state_buffer.unshift(ST); // marks the DTC as priority in the buffer, placing it first

  // else if (ST == Odometer_PID)
  //   return Verify_odometer_exist() ? state_buffer.push(ST) : 0;

  // else
  //   return Check_bin_for_state(ST) ? state_buffer.push(ST) : 0;
}

void save_flag_imu_parameter(bool _flag)
{
  imu_flag = _flag;
}

void save_flag_gps_parameter(bool _flag_)
{
  gps_flag = _flag_;
}

String verify_message_is_null(int id, double msg)
{
  return String(msg);
  // switch (id)
  // {
  //   case GPS_ST:
  //     return gps_flag ? String(msg) : "null";
  //     break;
    
  //   case Accelerometer_ST:
  //     return imu_flag ? String(msg) : "null";
  //     break;

  //   case Odometer_PID:
  //     return Verify_odometer_exist() ? String(msg) : "null";
  //     break;

  //   default:
  //     return Check_bin_for_state(id) ? String(msg) : "null";
  //     break;
  // }
}
