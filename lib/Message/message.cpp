#include "message.h"

bool debug_mode = false;
BLEmsg_t BLEmsg = defaultmsg();

void TratamentoDaMsg(unsigned char* info_can, int length)
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
   
   if(info_can[2] == 5) 
   {
      float A = info_can[3];        
      BLEmsg.EngineCollant = A-40;
      if(debug_mode) Serial.printf("EngineCollant:   %f", BLEmsg.EngineCollant);
   }

   if(info_can[2] == 12) 
   {
      float RPM1 = info_can[3], RPM2 = info_can[4];
      BLEmsg.Engine_rpm = ((RPM1*256)+RPM2)/4;
      if(debug_mode) Serial.printf("ENGINE_RPM:   %f", BLEmsg.Engine_rpm);
   }

   if(info_can[2] == 13) 
   {
      float A = info_can[3];        
      BLEmsg.vehicle_speed = A;
      if(debug_mode) Serial.printf("Vehicle_Speed:   %f", BLEmsg.vehicle_speed);
   }

   if(info_can[2] == 47) 
   {
      float A = info_can[3];        
      BLEmsg.fuellevel = (100*A)/255;
      if(debug_mode) Serial.printf("FuelLevel:   %f", BLEmsg.fuellevel);
   }

   if(info_can[2] == 49) 
   {
      float A = info_can[3], B = info_can[4];
      BLEmsg.Distance_travel = ((A*256)+B);
      if(debug_mode) Serial.printf("Distance_Traveled:   %f\r\n", BLEmsg.Distance_travel);
   }
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
