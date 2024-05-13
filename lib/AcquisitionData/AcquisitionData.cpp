#include "AcquisitionData.h"

/* Debug Variables */
bool debug_when_receive = true; // variable to enable the Serial when receive

bool Check_Current_State_Machine(void)
{
  uint8_t __ID = CircularBuffer_state();

  if(__ID==0x02)
    Serial.println("ACC");
  if(__ID==0x04)
    Serial.println("GPS");
  
  return __ID==0x00; // means this ID is the CAN msg (default in state machine)
}

/*================================ Accelerometer && GPS functions ================================*/
//acc
//gps

/*================================== CAN Acquisition functions ==================================*/
void MsgRec_Treatment()
{
  uint8_t length = 8;
  uint32_t ID = 0;
  unsigned char info_can[8] = {0};

  while(msg_receive())
  {
    get_msg(info_can, ID, length);

    if(debug_when_receive)
    {
      for(int i = 0; i < length; i++)
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
      if(info_can[3]==PIDs5) Storage_PIDenable_bit(info_can, PID_to_index_5);
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
