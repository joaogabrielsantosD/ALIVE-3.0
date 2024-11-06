#include "CircularBufferState.h"

//#define CIRCULAR_BUFFER_DEBUG

/* Variables for Circular Buffer */
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_pid = IDLE_ST;

/* Return the PID in the queue */
int CircularBuffer_state()
{
  #ifdef CIRCULAR_BUFFER_DEBUG
    printBuffer();
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
int insert(int ST, bool slow_msg)
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
void printBuffer()
{
  if (state_buffer.isEmpty())
    Serial.println("empty");

  else
  {
    Serial.print("[");
    for (decltype(state_buffer)::index_t i = 0; i < state_buffer.size() - 1; i++)
    {
      Serial.print(state_buffer[i], HEX);
      Serial.print(",");
    }
    Serial.print(state_buffer[state_buffer.size() - 1]);
    Serial.print("] (");

    Serial.print(state_buffer.size());
    Serial.print("/");
    Serial.print(state_buffer.size() + state_buffer.available());
    if (state_buffer.isFull())
      Serial.print(" full");

    Serial.println(")");
  }
}

String verify_message_is_null(int id, double msg)
{
  switch (id)
  {
    case GPS_ST:
      //return gps_flag ? String(msg) : "null";
      return String(msg);
      break;

    case Accelerometer_ST:
      //return imu_flag ? String(msg) : "null";
      return String(msg);
      break;

    case Odometer_PID:
      return Verify_odometer_exist() ? String(msg) : "null";
      break;

    default:
      return Check_bin_for_state(id) ? String(msg) : "null";
      break;
  }
}
