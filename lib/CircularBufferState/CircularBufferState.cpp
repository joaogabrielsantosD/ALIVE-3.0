#include "CircularBufferState.h"

/* Variables for Circular Buffer */
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_pid = IDLE_ST;
bool imu_flag = false, gps_flag = false;

/* Return the PID in the queue */
int CircularBuffer_state()
{
  if (state_buffer.isFull())
    current_pid = state_buffer.pop();

  else
  {
    if (!state_buffer.isEmpty())
      current_pid = state_buffer.pop();      
    else
      current_pid = IDLE_ST;
  }

  return current_pid;
}

/* Insert the PID in the queue */
int insert(int ST)
{
  switch (ST)
  {
    case DTC_mode_3:
      return state_buffer.unshift(ST); // marks the DTC as priority in the buffer, placing it first
      break;

    case Odometer_PID:
      return Verify_odometer_exist() ? state_buffer.push(ST) : 0;
      break;

    default:
      return Check_bin_for_state(ST) ? state_buffer.push(ST) : -1;
      break;
  }
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
