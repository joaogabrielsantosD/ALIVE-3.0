#include "StateMachine.h"

CircularBuffer<state_t, BUFFER_SIZE> state_buffer;
state_t current_state = IDLE_ST;

state_t CircularBuffer_state()
{
  bool buffer_full = false;

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
     
     //Serial.println("Entrou no Switchcase");
    case IDLE_ST:
      //Serial.println("i");
      break;
    
    case DistanceTraveled_ST:
    {   
      //Serial.println("Distance");
      unsigned char messageData[8] = {0x02, 0x01, DistanceTraveled_PID, 0x00, 0x00, 0x00, 0x00, 0x00};
      if(send_msg(messageData))
      {
        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for(int i = 0; i < 8; i++)
        {
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
        }
        Serial.println();
      }
      
      break;
    }

    case EngineRPM_ST:
    {
      //Serial.println("Entrou no 2");
      unsigned char messageData[8] = {0x02, 0x01, EngineRPM_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(send_msg(messageData))
      {
        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for(int i = 0; i < 8; i++)
        {
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
        }
        Serial.println();
      }
          
        break;
    }

    case VehicleSpeed_ST:
    {
      //Serial.println("Entrou no 3");

      unsigned char messageData[8] = {0x02, 0x01, VehicleSpeed_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(send_msg(messageData))
      {
        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for(int i = 0; i < 8; i++)
        {
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
        }
        Serial.println();
      }
      
      break;
    }

    case FuelLevel_ST:
    {
         // Serial.println("Entrou no 4");
      unsigned char messageData[8] = {0x02, 0x01, FuelLevel_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(send_msg(messageData))
      {
        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for(int i = 0; i < 8; i++)
        {
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
        }
        Serial.println();
      }
      
      break;
    }

    case EngineCoolant_ST:
    {
        //Serial.println("Entrou no 5");
      unsigned char messageData[8] = {0x02, 0x01, EngineCoolant_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(send_msg(messageData))
      {
        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for(int i = 0; i < 8; i++)
        {
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
        }
        Serial.println();
      } 
      
      break;
    }
  }

  return current_state;
}

bool insert(state_t ST)
{
  return state_buffer.push(ST);
}