#include "CircularBufferState.h"

// #define CIRCULAR_BUFFER_DEBUG

CircularBufferStateHandler CircularBufferState;

/* Return the PID in the queue */
int CircularBufferStateHandler::CircularBuffer_state()
{
#ifdef CIRCULAR_BUFFER_DEBUG
  this->printBuffer();
#endif

  if (state_buffer.isFull())
    current_pid = state_buffer.shift();

  else
  {
    if (!state_buffer.isEmpty())
      current_pid = state_buffer.shift();
    else
      current_pid = IDLE_ST;
  }

  return current_pid;
}

/* Insert the PID in the queue */
int CircularBufferStateHandler::insert(int ST, bool slow_msg)
{
  switch (ST)
  {
  case DTC_mode_3:
    return state_buffer.unshift(ST); // marks the DTC as priority in the buffer, placing it first
    break;

  case Odometer_PID:
    return Verify_odometer_exist() ? state_buffer.push(ST) : -1;
    break;

  default:
    if (slow_msg)
      return Check_bin_for_state(ST) ? state_buffer.unshift(ST) : -1;
    else
      return Check_bin_for_state(ST) ? state_buffer.push(ST) : -1;
    break;
  }

  vTaskDelay(5);
}

/* Print the CircularBuffer */
void CircularBufferStateHandler::printBuffer()
{
  if (this->state_buffer.isEmpty())
    Serial.println("empty");

  else
  {
    Serial.print("[");
    for (decltype(this->state_buffer)::index_t i = 0; i < this->state_buffer.size() - 1; i++)
    {
      Serial.print(this->state_buffer[i], HEX);
      Serial.print(",");
    }
    Serial.print(this->state_buffer[this->state_buffer.size() - 1]);
    Serial.print("] (");

    Serial.print(this->state_buffer.size());
    Serial.print("/");
    Serial.print(this->state_buffer.size() + this->state_buffer.available());
    if (this->state_buffer.isFull())
      Serial.print(" full");

    Serial.println(")");
  }
}

/* DEPRECAPTED  */
String CircularBufferStateHandler::verify_message_is_null(int id, double msg)
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