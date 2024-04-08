#include "message.h"

void TratamentoDaMsg(unsigned char* info_can, int length)
{
   for(int i = 0; i < length; i++)
   {
      Serial.print((info_can[i]),HEX); 
      Serial.print("\t");
   }
   Serial.println();

   if(info_can[2] == 49) 
   {
      float A;
      float B;
      A = info_can[3] ;
      B = info_can[4] ;
      float Distance_Traveled = ((A*256)+B);
      Serial.print("Distance_Traveled:   ");
      Serial.println(Distance_Traveled);
   }


   if(info_can[2] == 12) 
   {
      float RPM1;
      float RPM2;
      RPM1 = info_can[3] ;
      RPM2 = info_can[4] ;
      float ENGINE_RPM = ((RPM1*256)+RPM2)/4;
      Serial.print("ENGINE_RPM:   ");
      Serial.println(ENGINE_RPM);
   }

   if(info_can[2] == 13) 
   {
      float A;
      A = info_can[3];        
      float Vehicle_Speed = A;
      Serial.print("Vehicle_Speed:   ");
      Serial.println(Vehicle_Speed);
   }

   if(info_can[2] == 47) 
   {
      float A;
      A = info_can[3];        
      float FuelLevel = (100*A)/255;
      Serial.print("FuelLevel:   ");
      Serial.println(FuelLevel);
   }

   if (info_can[2] == 5) 
   {
      float A;
      A = info_can[3];        
      float EngineCollant = A-40;
      Serial.print("EngineCollant:   ");
      Serial.println(EngineCollant);
   }
}