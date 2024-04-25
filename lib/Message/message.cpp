#include "message.h"

bool debug_mode = true;
uint8_t PID_enable_bit[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t PID_Enables_bin[128];
BLEmsg_t BLEmsg = defaultmsg();

void MsgRec_Treatment(unsigned char* info_can, int length)
{
   if(debug_mode)
   {
      for(int i = 0; i < length; i++)
      {
         Serial.print((info_can[i]), HEX); 
         Serial.print("\t");
      }
      Serial.println();
   }

   if(info_can[2]==0x41 && info_can[0]==0x10)
   {
      if(info_can[3]==PIDsupported1) Storage_PIDenable_bit(info_can, PID_to_index_1);
      if(info_can[3]==PIDsupported2) Storage_PIDenable_bit(info_can, PID_to_index_2);
      if(info_can[3]==PIDsupported3) Storage_PIDenable_bit(info_can, PID_to_index_3);
      if(info_can[3]==PIDsupported4) Storage_PIDenable_bit(info_can, PID_to_index_4);
   }

   else if(info_can[2] == 5) 
   {
      float A = info_can[3];        
      BLEmsg.EngineCollant = A-40;
      if(debug_mode) Serial.printf("EngineCollant:   %f", BLEmsg.EngineCollant);
   }

   else if(info_can[2] == 12) 
   {
      float RPM1 = info_can[3], RPM2 = info_can[4];
      BLEmsg.Engine_rpm = ((RPM1*256)+RPM2)/4;
      if(debug_mode) Serial.printf("ENGINE_RPM:   %f", BLEmsg.Engine_rpm);
   }

   else if(info_can[2] == 13) 
   {
      float A = info_can[3];        
      BLEmsg.vehicle_speed = A;
      if(debug_mode) Serial.printf("Vehicle_Speed:   %f", BLEmsg.vehicle_speed);
   }

   else if(info_can[2] == 47) 
   {
      float A = info_can[3];        
      BLEmsg.fuellevel = (100*A)/255;
      if(debug_mode) Serial.printf("FuelLevel:   %f", BLEmsg.fuellevel);
   }

   else if(info_can[2]==DistanceTraveled_PID) 
   {
      float A = info_can[3], B = info_can[4];
      BLEmsg.Distance_travel = ((A*256)+B);
      if(debug_mode) Serial.printf("Distance_Traveled:   %f\r\n", BLEmsg.Distance_travel);
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
{  int k = 7;
   int k2 = 7;

   for(int i = 0; i < 16; i++)
   {  int j = 0; 
      if(i>0){k = k2+8;
              k2 = k;}   

      uint8_t Aux = PID_enable_bit[i];

      /*
      Serial.printf("\n"); 
      Serial.printf("Message:\n"); 
      Serial.printf("%d", PID_enable_bit[i]);
      Serial.printf("\n"); 
      */

      while(j < 8)
      {  PID_Enables_bin[k] = Aux % 2;                
         Aux = Aux / 2;
         //Serial.printf("%d", PID_Enables_bin[k]);   
         k--;
         j++;            
      } 

      Serial.println(); 
      Serial.println("Vetor Bin: ");      
      for(int l=0; l < 8; l++){Serial.printf("%d", PID_Enables_bin[l]);}       
   }  
   
   Serial.println();
   for(int l=0; l < 128; l++){Serial.printf("%d", PID_Enables_bin[l]);}    
   
}

int Check_bin_for_state(int pid_order)
{
   return PID_Enables_bin[pid_order-1] & 0x01;
}

BLEmsg_t defaultmsg()
{
   BLEmsg_t t;
   memset(&t, 0, sizeof(BLEmsg_t));
   return t;
}

BLEmsg_t requestMsg()
{
   return BLEmsg;
}
