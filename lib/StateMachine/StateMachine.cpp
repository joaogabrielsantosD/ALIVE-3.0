#include "StateMachine.h"

bool DebugMode = false;
CircularBuffer<state_t, BUFFER_SIZE> state_buffer;
state_t current_state = IDLE_ST;

state_t CircularBuffer_state()
{
  bool buffer_full = false;
  unsigned char messageData[8] = {0x02, 0x01, 0x00/*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  if(state_buffer.isFull())
  {
    buffer_full = true;
    current_state = state_buffer.pop();
  } else {
    buffer_full = false;
    if(!state_buffer.isEmpty())
      current_state = state_buffer.pop();        
    else
      current_state = IDLE_ST;
  }

  switch(current_state) 
  {     
    case IDLE_ST:
      //Serial.println("i");
      break;
    
    case DistanceTraveled_ST:
    {   
      messageData[2] = DistanceTraveled_PID;
      
      if(send_msg(messageData) && DebugMode) debug_print(messageData);
      
      break;
    }

    case EngineRPM_ST:
    {
      messageData[2] = EngineRPM_PID;      
        
      if(send_msg(messageData) && DebugMode) debug_print(messageData);
      
      break;
    }

    case VehicleSpeed_ST:
    {
      messageData[2] = VehicleSpeed_PID;

      if(send_msg(messageData) && DebugMode) debug_print(messageData);
      
      break;
    }

    case FuelLevel_ST:
    {
      messageData[2] = FuelLevel_PID; 
             
      if(send_msg(messageData) && DebugMode) debug_print(messageData);

      break;
    }

    case EngineCoolant_ST:
    {
      messageData[2] = EngineCoolant_PID;

      if(send_msg(messageData) && DebugMode) debug_print(messageData);
      
      break;
    }
  }

  return current_state;
}

bool insert(state_t ST)
{
  if(Check_bin_for_state((int)ST)) 
    return state_buffer.push(ST);
  else
    return false;
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
