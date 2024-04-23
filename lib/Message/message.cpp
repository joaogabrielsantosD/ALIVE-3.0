#include "message.h"

bool debug_mode = true;
//uint8_t PID_enable_bit[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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

   if(info_can[3]==0x00 && info_can[2]==0x41)
   {
      Serial.println("PID 1!");
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

   else if(info_can[2] == DistanceTraveled_PID) 
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
