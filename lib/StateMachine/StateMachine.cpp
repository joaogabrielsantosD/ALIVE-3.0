#include "StateMachine.h"

bool DebugMode = false;
CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_id = 0;

int CircularBuffer_state()
{
  bool buffer_full = false;
  unsigned char messageData[8] = {0x02, 0x01, 0x00/*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  if(state_buffer.isFull())
  {
    buffer_full = true;
    current_id = state_buffer.pop();
  } else {
    buffer_full = false;
    if(!state_buffer.isEmpty())
      current_id = state_buffer.pop();        
    else
      current_id = 0;
  }

  switch(current_id) 
  {
    case 0:
      //Serial.println("i");
      break;
      
    default:
      messageData[2] = current_id;
      
      if(send_msg(messageData) && DebugMode) debug_print(messageData);

      break;
  }

  return current_id;
}

bool insert(int ST)
{
  return (Check_bin_for_state(ST) ? state_buffer.push(ST) : 0);
}

void debug_print(unsigned char* message)
{
  Serial.print("Send to CAN: id ");
  Serial.print(CAN_ID, HEX);
  Serial.print("  ");  
  for(int i = 0; i < 8; i++)
  {
    Serial.print((message[i]), HEX); 
    Serial.print("\t");
  }
  Serial.println();
}
