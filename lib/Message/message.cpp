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
   } // atualizar recebimento da CAN 

   if(info_can[2]==0x41 && info_can[0]==0x10)
   {
      if(info_can[3]==PIDsupported1) Storage_PIDenable_bit(info_can, PID_to_index_1);
      if(info_can[3]==PIDsupported2) Storage_PIDenable_bit(info_can, PID_to_index_2);
      if(info_can[3]==PIDsupported3) Storage_PIDenable_bit(info_can, PID_to_index_3);
      if(info_can[3]==PIDsupported4) Storage_PIDenable_bit(info_can, PID_to_index_4);
   }

   else if(info_can[2]==MonitorStatus)
   {
      Serial.printf("monitor status\r\n");
   }

   else if(info_can[2]==FreezeDTC)
   {
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==FuelSystem)
   {
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==EngineLoad)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("EngineLoad:   %f\r\n", res);
   }

   else if(info_can[2]==EngineCollantTemp) 
   {
      float A = info_can[3];        
      BLEmsg.EngineCollant = A-40;
      if(debug_mode) Serial.printf("EngineCollant:   %f\r\n", BLEmsg.EngineCollant);
   }

   else if(info_can[2]==ShortTermFuel_Bank1)
   {
      float A = info_can[3];        
      float res = (A-128)*(100/128);
      if(debug_mode) Serial.printf("ShortTermFuel_Bank1:   %f\r\n", res);
   }

   else if(info_can[2]==LongTermFuel_Bank1)
   {
      float A = info_can[3];        
      float res = (A-128)*(100/128);
      if(debug_mode) Serial.printf("LongTermFuel_Bank1:   %f\r\n", res);
   }

   else if(info_can[2]==ShortTermFuel_Bank2)
   {
      float A = info_can[3];        
      float res = (A-128)*(100/128);
      if(debug_mode) Serial.printf("ShortTermFuel_Bank2:   %f\r\n", res);
   }

   else if(info_can[2]==LongTermFuel_Bank2)
   {
      float A = info_can[3];        
      float res = (A-128)*(100/128);
      if(debug_mode) Serial.printf("LongTermFuel_Bank2:   %f\r\n", res);
   }

   else if(info_can[2]==FuelPressure)
   {
      float A = info_can[3];        
      float res = A*3;
      if(debug_mode) Serial.printf("FuelPressure:   %f\r\n", res);
   }

   else if(info_can[2]==AbsolutePressure)
   {
      float A = info_can[3];        
      float res = A;
      if(debug_mode) Serial.printf("AbsolutePressure:   %f\r\n", res);
   }

   else if(info_can[2]==EngineRPM)
   {
      float A = info_can[3], B = info_can[4];        
      float res = ((A*256)+B)/4;
      if(debug_mode) Serial.printf("EngineRPM:   %f\r\n", res);
   }

   else if(info_can[2]==VehicleSpeed)
   {
      float A = info_can[3];        
      float res = A;
      if(debug_mode) Serial.printf("VehicleSpeed:   %f\r\n", res);
   }

   else if(info_can[2]==TimingAdvance)
   {
      float A = info_can[3];        
      float res = (A/2) - 64;
      if(debug_mode) Serial.printf("TimingAdvance:   %f\r\n", res);
   }

   else if(info_can[2]==IntakeAirTemperature)
   {
      float A = info_can[3];        
      float res = A-40;
      if(debug_mode) Serial.printf("IntakeAirTemperature:   %f\r\n", res);
   }

   else if(info_can[2]==MAFairFlowRate)
   {
      float A = info_can[3], B = info_can[4];        
      float res = ((A*256)+B)/100;
      if(debug_mode) Serial.printf("MAFairFlowRate:   %f\r\n", res);
   }

   else if(info_can[2]==ThrottlePosition)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("ThrottlePosition:   %f\r\n", res);
   }

   else if(info_can[2]==CommandedSecondaryAirStatus)
   {
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==OxygenSensorsPresent)
   {
      Serial.printf("Oxygen present\r\n");
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4)
   {
      float A = info_can[3], B = info_can[4];
      float res = A/200;
      if(B!=0xFF)
      {
         float res0 = (B-128)*(100/128);
         if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1( 2 ):   %f\r\n", res0);
      }
      if(debug_mode) Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==OBDstandard)
   {
      Serial.printf("obd standard\r\n");
   }

   else if(info_can[2]==OxygenSensorsPresent2)
   {
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==AuxiliaryInputStatus)
   {
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==RunTime)
   {
      float A = info_can[3], B = info_can[4];        
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("RunTime:   %f\r\n", res);
   }

/*=======================================================================================================================*/
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
