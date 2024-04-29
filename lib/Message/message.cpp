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
   { // !<Bit enconde>
      Serial.printf("monitor status\r\n");
   }

   else if(info_can[2]==FreezeDTC)
   { // !<Bit enconde>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==FuelSystem)
   { // !<Bit enconde>s
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
   {  // !<Bit enconde>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==OxygenSensorsPresent)
   {  // !<Bit enconde>
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
   {  // !<Bit enconde>
      Serial.printf("obd standard\r\n");
   }

   else if(info_can[2]==OxygenSensorsPresent2)
   {  // !<Bit enconde>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==AuxiliaryInputStatus)
   {  // !<Bit enconde>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==RunTime)
   {
      float A = info_can[3], B = info_can[4];        
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("RunTime:   %f\r\n", res);
   }

   else if(info_can[2]==DistanceTraveled)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("DistanceTraveled:   %f\r\n", res);
   }

   else if(info_can[2]==FuelRailPressure_vac)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)*0.079;
      if(debug_mode) Serial.printf("FuelRailPressure_vac:   %f\r\n", res);
   }

   else if(info_can[2]==FuelRailPressure_dis)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)*10;
      if(debug_mode) Serial.printf("FuelRailPressure_dis:   %f\r\n", res);
   }

   else if(info_can[2]==O2S1_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535);
      if(debug_mode) Serial.printf("O2S1_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S2_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535);
      if(debug_mode) Serial.printf("O2S2_WR_lambda1:   %f | %f\r\n", res, res0); 
   }

   else if(info_can[2]==O2S3_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535); 
      if(debug_mode) Serial.printf("O2S3_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S4_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535);
      if(debug_mode) Serial.printf("O2S4_WR_lambda1:   %f | %f\r\n", res, res0); 
   }

   else if(info_can[2]==O2S5_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535); 
      if(debug_mode) Serial.printf("O2S5_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S6_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535); 
      if(debug_mode) Serial.printf("O2S6_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S7_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535); 
      if(debug_mode) Serial.printf("O2S7_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S8_WR_lambda1)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[2];
      float res = ((A*256)+B)*(2/65535), res0 = ((C*256)+D)*(8/65535); 
      if(debug_mode) Serial.printf("O2S8_WR_lambda1:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==EGR)
   {
      float A = info_can[3];
      float res = 100*(A/255); 
      if(debug_mode) Serial.printf("EGR:   %f\r\n", res);
   }

   else if(info_can[2]==EGR_ERROR)
   {
      float A = info_can[3];
      float res = (A-128)*(100/128);
      if(debug_mode) Serial.printf("EGR_ERROR:   %f\r\n", res);
   }

   else if(info_can[2]==EvaporativePurge)
   {
      float A = info_can[3];
      float res = 100*(A/255);
      if(debug_mode) Serial.printf("EvaporativePurge:   %f\r\n", res);
   }

   else if(info_can[2]==FuelLevelInput)
   {
      float A = info_can[3];
      float res = 100*(A/255);
      if(debug_mode) Serial.printf("FuelLevelInput:   %f\r\n", res);
   }

   else if(info_can[2]==warm_ups)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("warm_ups:   %f\r\n", res);
   }

   else if(info_can[2]==DistanceTraveledSinceCodeCleared) 
   {
      float A = info_can[3], B = info_can[4];
      BLEmsg.Distance_travel = ((A*256)+B);
      if(debug_mode) Serial.printf("Distance_Traveled:   %f\r\n", BLEmsg.Distance_travel);
   }

   else if(info_can[2]==VaporPressure)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)/4;  
      if(debug_mode) Serial.printf("VaporPressure:   %f\r\n", res);
   }

   else if(info_can[2]==BarometricPressure)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("BarometricPressure:   %f\r\n", res);
   }

   else if(info_can[2]==O2S1_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S1_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S2_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S2_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S3_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S3_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S4_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S4_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S5_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S5_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S6_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S6_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S7_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S7_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==O2S8_WR_lambda2)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = ((A*256)+B)/32.768, res0 = (((C*256)+D)/256) - 128; 
      if(debug_mode) Serial.printf("O2S8_WR_lambda2:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==CatalystTemperature_Bank1Sensor1)
   {  
      float A = info_can[3], B = info_can[4];
      float res = (((A*256)+B)/10) - 40;
      if(debug_mode) Serial.printf("CatalystTemperature_Bank1Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==CatalystTemperature_Bank2Sensor1)
   {  
      float A = info_can[3], B = info_can[4];
      float res = (((A*256)+B)/10) - 40;
      if(debug_mode) Serial.printf("CatalystTemperature_Bank2Sensor1:   %f\r\n", res);
   }

   else if(info_can[2]==CatalystTemperature_Bank1Sensor2)
   {  
      float A = info_can[3], B = info_can[4];
      float res = (((A*256)+B)/10) - 40;
      if(debug_mode) Serial.printf("CatalystTemperature_Bank1Sensor2:   %f\r\n", res);
   }

   else if(info_can[2]==CatalystTemperature_Bank2Sensor2)
   {  
      float A = info_can[3], B = info_can[4];
      float res = (((A*256)+B)/10) - 40;
      if(debug_mode) Serial.printf("CatalystTemperature_Bank2Sensor2:   %f\r\n", res);
   }

   else if(info_can[2]==MonitorStatusDriveCycle)
   {  // !<Bit encode>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==ControlModuleVoltage)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)/1000;
      if(debug_mode) Serial.printf("ControlModuleVoltage:   %f\r\n", res);
   }

   else if(info_can[2]==AbsoluteLoadValue)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)*(100/255);
      if(debug_mode) Serial.printf("AbsoluteLoadValue:   %f\r\n", res);
   }

   else if(info_can[2]==CommandEquivalenceRatio)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)/32.768;
      if(debug_mode) Serial.printf("CommandEquivalenceRatio:   %f\r\n", res);
   }

   else if(info_can[2]==RelativeThrottlePosition)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("RelativeThrottlePosition:   %f\r\n", res);
   }

   else if(info_can[2]==AmbientAirTemperature)
   {
      float A = info_can[3];
      float res = A-40;
      if(debug_mode) Serial.printf("AmbientAirTemperature:   %f\r\n", res);
   }

   else if(info_can[2]==AbsoluteThrottlePositionB)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("AbsoluteThrottlePositionB:   %f\r\n", res);
   }

   else if(info_can[2]==AbsoluteThrottlePositionC)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("AbsoluteThrottlePositionC:   %f\r\n", res);
   }

   else if(info_can[2]==AcceleratorPedalD)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("AcceleratorPedalD:   %f\r\n", res);
   }

   else if(info_can[2]==AcceleratorPedalE)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("AcceleratorPedalE:   %f\r\n", res);
   }

   else if(info_can[2]==AcceleratorPedalF)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("AcceleratorPedalF:   %f\r\n", res);
   }

   else if(info_can[2]==CommandedThrottleActuator)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("CommandedThrottleActuator:   %f\r\n", res);
   }

   else if(info_can[2]==TimeRun_MIL)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("TimeRun_MIL:   %f\r\n", res);
   }

   else if(info_can[2]==TimeSinceTroubleCodesCleared)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("TimeSinceTroubleCodesCleared:   %f\r\n", res);
   }

   else if(info_can[2]==MaximumValueForEquivalenceRatio)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = A, res0 = B, res1 = C, res2 = D*10;
      if(debug_mode) Serial.printf("MaximumValueForEquivalenceRatio:   %f | %f | %f | %f\r\n", res, res0, res1, res2);
   }

   else if(info_can[2]==MaximumValueForAirFlowRate)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = A*10, res0 = B, res1 = C;
      if(debug_mode) Serial.printf("MaximumValueForEquivalenceRatio:   %f | %f | %f\r\n", res, res0, res1);
   }

   else if(info_can[2]==FuelType)
   {
      uint16_t A = info_can[3];
      String res = Check_type_of_fuel(A);
      if(debug_mode) Serial.printf("FuelType:   %s\r\n", res);
   }

   else if(info_can[2]==EthanolFuel)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("EthanolFuel:   %f \r\n", res);
   }

   else if(info_can[2]==AbsoluteVapourPressure)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = (A/200) + (B/200) + (C/200) + (D/200);
      if(debug_mode) Serial.printf("AbsoluteVapourPressure:   %f\r\n", res);
   }

   else if(info_can[2]==EvapSystemVaporPressure)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B) - 32.768;
      if(debug_mode) Serial.printf("EvapSystemVaporPressure:   %f\r\n", res);
   }

   else if(info_can[2]==ShortTermSecondaryOxygenSensor_bank1bank3)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A-128)*(100/128), res0 = (B-128)*(100/128);
      if(debug_mode) Serial.printf("ShortTermSecondaryOxygenSensor_bank1bank3:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==LongTermSecondaryOxygenSensor_bank1bank3)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A-128)*(100/128), res0 = (B-128)*(100/128);
      if(debug_mode) Serial.printf("LongTermSecondaryOxygenSensor_bank1bank3:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==ShortTermSecondaryOxygenSensor_bank2bank4)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A-128)*(100/128), res0 = (B-128)*(100/128);
      if(debug_mode) Serial.printf("ShortTermSecondaryOxygenSensor_bank2bank4:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==LongTermSecondaryOxygenSensor_bank2bank4)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A-128)*(100/128), res0 = (B-128)*(100/128);
      if(debug_mode) Serial.printf("LongTermSecondaryOxygenSensor_bank2bank4:   %f | %f\r\n", res, res0);
   }

   else if(info_can[2]==AbsoluteFuelRailPressure)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)*10;
      if(debug_mode) Serial.printf("AbsoluteFuelRailPressure:   %f \r\n", res);
   }

   else if(info_can[2]==RelativeAcceleratorPedalPosition)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("RelativeAcceleratorPedalPosition:   %f \r\n", res);
   }

   else if(info_can[2]==HybridBatteryLife)
   {
      float A = info_can[3];
      float res = (A*100)/255;
      if(debug_mode) Serial.printf("HybridBatteryLife:   %f \r\n", res);
   }

   else if(info_can[2]==EngineOilTemperature)
   {
      float A = info_can[3];
      float res = A-40;
      if(debug_mode) Serial.printf("EngineOilTemperature:   %f \r\n", res);
   }

   else if(info_can[2]==FuelInjectionTiming)
   {
      float A = info_can[3], B = info_can[4];
      float res = (((A*256)+B)-26.880)/128;
      if(debug_mode) Serial.printf("FuelInjectionTiming:   %f \r\n", res);
   }

   else if(info_can[2]==EngineFuelRate)
   {
      float A = info_can[3], B = info_can[4];
      float res = ((A*256)+B)*0.05;
      if(debug_mode) Serial.printf("EngineFuelRate:   %f \r\n", res);
   }

   else if(info_can[2]==EmissionRequirements)
   {  //!<Bit encode>
      Serial.printf("%d %d %d %d\r\n", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==DriverDemandEngine)
   {
      float A = info_can[3];
      float res = A-125;
      if(debug_mode) Serial.printf("DriverDemandEngine:   %f \r\n", res);
   }

   else if(info_can[2]==ActualEngine_PercentTorque)
   {
      float A = info_can[3];
      float res = A-125;
      if(debug_mode) Serial.printf("ActualEngine_PercentTorque:   %f \r\n", res);
   }

   else if(info_can[2]==EngineReferenceTorque)
   {
      float A = info_can[3], B = info_can[4];
      float res = (A*256)+B;
      if(debug_mode) Serial.printf("EngineReferenceTorque:   %f \r\n", res);
   }

   else if(info_can[2]==EnginePercentTorque)
   {
      float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
      float res = A-125, res0 = B-125, res1 = C-125, res2 = D-125;
      if(debug_mode) Serial.printf("EnginePercentTorque:   %f | %f | %f | %f\r\n", res, res0, res1, res2);
   }

   else if(info_can[2]==AuxiliaryInput)
   {  // !<Bit encode>
      Serial.printf("%d %d %d %d", info_can[3], info_can[4], info_can[5], info_can[6]);
   }

   else if(info_can[2]==MassAirFlowSensor)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("MassAirFlowSensor:   %f \r\n", res);
   }

   else if(info_can[2]==EngineCoolantTemperature)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("EngineCoolantTemperature:   %f \r\n", res);
   }

   else if(info_can[2]==IntakeAirTemperatureSensor)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("IntakeAirTemperatureSensor:   %f \r\n", res);
   }

   else if(info_can[2]==CommandedEGR_ERROR)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("CommandedEGR_ERROR:   %f \r\n", res);
   }

   else if(info_can[2]==CommandedDiesel)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("CommandedDiesel:   %f \r\n", res);
   }

   else if(info_can[2]==ExhaustGas)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("ExhaustGas:   %f \r\n", res);
   }

   else if(info_can[2]==CommandedThrottleActuator2)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("CommandedThrottleActuator2:   %f \r\n", res);
   }

   else if(info_can[2]==FuelPressureControlSystem)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("FuelPressureControlSystem:   %f \r\n", res);
   }

   else if(info_can[2]==InjectionPressureControl)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("InjectionPressureControl:   %f \r\n", res);
   }

   else if(info_can[2]==TurboChargerCompressor)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("TurboChargerCompressor:   %f \r\n", res);
   }

   else if(info_can[2]==BoostPressureControl)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("BoostPressureControl:   %f \r\n", res);
   }

   else if(info_can[2]==VGT)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("VGT:   %f \r\n", res);
   }

   else if(info_can[2]==WastegateControl)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("WastegateControl:   %f \r\n", res);
   }

   else if(info_can[2]==ExhaustPressure)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("ExhaustPressure:   %f \r\n", res);
   }

   else if(info_can[2]==TurbochargerRPM)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("TurbochargerRPM:   %f \r\n", res);
   }

   else if(info_can[2]==TurbochargerTemperature1)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("TurbochargerTemperature1:   %f \r\n", res);
   }

   else if(info_can[2]==TurbochargerTemperature2)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("TurbochargerTemperature2:   %f \r\n", res);
   }

   else if(info_can[2]==ChargeAIR_CACT)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("ChargeAIR_CACT:   %f \r\n", res);
   }

   else if(info_can[2]==EGT_Bank1)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("EGT_Bank1:   %f \r\n", res);
   }

   else if(info_can[2]==EGT_Bank2)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("EGT_Bank2:   %f \r\n", res);
   }

   else if(info_can[2]==DPF1)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("DPF1:   %f \r\n", res);
   }

   else if(info_can[2]==DPF2)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("DPF2:   %f \r\n", res);
   }

   else if(info_can[2]==DPF_Temperature)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("DPF_Temperature:   %f \r\n", res);
   }

   else if(info_can[2]==NOxNTE)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("NOxNTE:   %f \r\n", res);
   }

   else if(info_can[2]==PMxNTE)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("PMxNTE:   %f \r\n", res);
   }

   else if(info_can[2]==EngineRunTime)
   {
      float A = info_can[3];
      float res = A;
      if(debug_mode) Serial.printf("EngineRunTime:   %f \r\n", res);
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

String Check_type_of_fuel(uint16_t type)
{
   switch(type)
   {
      case 0x01:
      {
         return "Gasoline";
         break;
      }

      case 0x02:
      {
         return "Methanol";
         break;
      }

      case 0x03:
      {
         return "Ethanol";
         break;
      }

      case 0x04:
      {
         return "Diesel";
         break;
      }

      case 0x05:
      {
         return "LPG";
         break;
      }

      case 0x06:
      {
         return "CNG";
         break;
      }

      case 0x07:
      {
         return "Propane";
         break;
      }

      case 0x08:
      {
         return "Electric";
         break;
      }

      case 0x09:
      {
         return "Bifuel run Gasoline";
         break;
      }

      case 0x0A:
      {
         return "Bifuel run Methanol";
         break;
      }

      case 0x0B:
      {
         return "Bifuel run Ethanol";
         break;
      }

      case 0x0C:
      {
         return "Bifuel run LPG";
         break;
      }

      case 0x0D:
      {
         return "Bifuel run CNG";
         break;
      }

      case 0x0E:
      {
         return "Bifuel run Prop";
         break;
      }

      case 0x0F:
      {
         return "Bifuel run Electricity";
         break;
      }

      case 0x10:
      {
         return "Bifuel mixed gas/electric";
         break;
      }

      case 0x11:
      {
         return "Hybrid gasoline";
         break;
      }

      case 0x12:
      {
         return "Hybrid ethanol";
         break;
      }

      case 0x13:
      {
         return "Hybrid Diesel";
         break;
      }

      case 0x14:
      {
         return "Hybrid electric";
         break;
      }

      case 0x15:
      {
         return "Hybrid mixed fuel";
         break;
      }

      case 0x16:
      {
         return "Hybrid regenerative";
         break;
      }

   }

   return "";
};

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
