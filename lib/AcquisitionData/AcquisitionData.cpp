#include "AcquisitionData.h"

/* Debug Variables */
bool debug_when_send = false;   // variable to enable the Serial when send the message
bool debug_when_receive = true; // variable to enable the Serial when receive
/* Variebles for Circular Buffer*/
CircularBuffer<int, BUFFER_SIZE*2> state_buffer;
int current_id = 0;
/* Variables for CAN */
uint8_t PID_enable_bit[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t PID_Enables_bin[128];
uint8_t pid5_enable = 0;

/*================================== Circular Buffer Functions ==================================*/
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
    
    case 1:
      //Serial.println("acc"); 
      break;
    
    case 2:
      //Serial.println("gps"); 
      break;

    default:
      messageData[2] = (unsigned char)current_id;
      
      if(send_msg(messageData) && debug_when_send) debug_print(messageData);

      break;
  }

  return current_id;
}

bool insert(int ST)
{
  if(ST==Accelerometer_ST || ST==GPS_ST) return state_buffer.push(ST);
  if(ST==Odometer_PID) return (Verify_odometer_exist() ? state_buffer.push(ST) : 0);

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

/*================================ Accelerometer && GPS functions ================================*/
//acc
//gps

/*================================== CAN Acquisition functions ==================================*/
void MsgRec_Treatment()
{
  uint8_t length = 0;
  uint32_t ID = 0;
  unsigned char info_can[8] = {0};

  while(msg_receive())
  {
    get_msg(info_can, ID, length);

    if(debug_when_receive)
    {
      for(int i=0; i < length; i++)
      {
        Serial.print(info_can[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    }

    if(info_can[2]==0x41 && info_can[0]==0x10)
    {
      if(info_can[3]==PIDs1) Storage_PIDenable_bit(info_can, PID_to_index_1);
      if(info_can[3]==PIDs2) Storage_PIDenable_bit(info_can, PID_to_index_2);
      if(info_can[3]==PIDs3) Storage_PIDenable_bit(info_can, PID_to_index_3);
      if(info_can[3]==PIDs4) Storage_PIDenable_bit(info_can, PID_to_index_4);
      if(info_can[3]==PIDs5) pid5_enable = ((info_can[4] >> 2) & ~0xFE); // move to 1 and disable the others bit
    }

    //if(info_can[2]==FueL_Status_PID)
    //{
    //  /* code */
    //}

    if(info_can[2]==Fuel_Pressure_PID)
    {
      float A = info_can[4];
      float res = 3*A;
      Serial.printf("Fuel Pressure:  %f\r\n", res);
    }

    if(info_can[2]==Fuel_Level_PID)
    {
      float A = info_can[4];
      float res = (100*A)/255;
      Serial.printf("Fuel Level Input:  %f\r\n", res);
    }

    if(info_can[2]==Speed_PID)
    {
      float A = info_can[4];
      float res = A;
      Serial.printf("Vehicle speed:  %f\r\n", res);
    }

    if(info_can[2]==Engine_LoadP_ID)
    {
      float A = info_can[4];
      float res = (100*A)/255;
      Serial.printf("Calculated engine load value:  %f\r\n", res);
    }

    if(info_can[2]==Engine_CoolantP_ID)
    {
      float A = info_can[4];
      float res = A - 40;
      Serial.printf("Engine Coolant Temperature:  %f\r\n", res);
    }
  
    if(info_can[2]==Engine_RPM_ID)
    {
      float A = info_can[4], B = info_can[5];
      float res = (256*A + B)/4;
      Serial.printf("Engine RPM:  %f\r\n", res);
    }

    if(info_can[2]==Run_Time_PID)
    {
      float A = info_can[4], B = info_can[5];
      float res = 256*A + B;
      Serial.printf("Run Time since engine start:  %f\r\n", res);
    }

    if(info_can[2]==Engine_Oil_PID)
    {
      float A = info_can[4];
      float res = A - 40;
      Serial.printf("Engine Oil Temperature:  %f\r\n", res);
    }

    if(info_can[2]==Engine_FuelRate_PID)
    {
      float A = info_can[4], B = info_can[5];
      float res = (256*A + B)/20;
      Serial.printf("Engine Fuel rate:  %f\r\n", res);
    }

    if(info_can[2]==Ambient_Temp_PID)
    {
      float A = info_can[4];
      float res = A - 40;
      Serial.printf("Ambient air temperature:  %f\r\n", res);
    }

    if(info_can[2]==Throttle_Pos_PID)
    {
      float A = info_can[4];
      float res = (100*A)/255;
      Serial.printf("Throttle position:  %f\r\n", res);
    }

    if(info_can[2]==Distance_on_MIL_PID)
    {
      float A = info_can[4], B = info_can[5];
      float res = 256*A + B;
      Serial.printf("Distance traveled with malfunction indicator lamp (MIL) on :  %f\r\n", res);
    }

    if(info_can[2]==Distance_Travel_PID)
    {
      float A = info_can[4], B = info_can[5];
      float res = 256*A + B;
      Serial.printf("Distance traveled since codes cleared:  %f\r\n", res);
    }

    if(info_can[2]==MAP_sensor_PID)
    {
      float A = info_can[4];
      float res = A;
      Serial.printf("Intake manifold absolute pressure(MAP):  %f\r\n", res);
    }

    if(info_can[2]==Odometer_PID)
    {
      float A = info_can[4], B = info_can[5], C = info_can[6], D = info_can[7];
      float res = ((A*pow(2,24)) + (B*pow(2,16)) + (C*pow(2,8)) + D)/10;
      Serial.printf("Odometer:  %f\r\n", res);
    }
  }
}

void Storage_PIDenable_bit(unsigned char* bit_data, int8_t position)
{
  PID_enable_bit[position]   = bit_data[4];
  PID_enable_bit[position+1] = bit_data[5];
  PID_enable_bit[position+2] = bit_data[6];
  PID_enable_bit[position+3] = bit_data[7];

  if(position==PID_to_index_4) Convert_Dec2Bin();
}

void Convert_Dec2Bin()
{  
  int k = 7, k2 = 7;

  for(int i = 0; i < 16; i++)
  {  
    int j = 0; 

    if(i > 0)
    {
      k = k2 + 8;
      k2 = k;
    }   
    uint8_t Aux = PID_enable_bit[i];
    
    while(j < 8)
    {  
      PID_Enables_bin[k] = Aux % 2;                
      Aux /= 2;
      k--;
      j++;            
    }      
  }   
}

int Check_bin_for_state(int pid_order)
{
  return PID_Enables_bin[pid_order-1] & 0x01;
}

int Verify_odometer_exist()
{
  return pid5_enable;
}
