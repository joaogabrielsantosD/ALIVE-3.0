#include "tickerISR.h"

bool Print_in_serial = true; // Put True or False to enable SerialPrint of checkPID
Ticker ticker200mHz, 
    ticker300mHz, 
    ticker1Hz,
    ticker2Hz,
    ticker10Hz;

void init_tickers()
{
  do 
  {
    Serial.println("Check the PID support...");
  } while(checkPID());
  
  ticker200mHz.attach(1.0/0.2, ticker200mHzISR); // 5s
  ticker300mHz.attach(1.0/0.3, ticker300mHzISR); // 3s
  ticker1Hz.attach(1.0, ticker1HzISR);           // 1s
  ticker2Hz.attach(1.0/2, ticker2HzISR);         // 0.5s
  ticker10Hz.attach(1.0/10, ticker10HzISR);      // 0.1s
}

bool checkPID()
{
  // Flag to check if you received the PID support message
  bool check_receive_pid = false;
  unsigned char Data[8] = {0x04, 0x01, 0x00/*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  for(int i = 1; i < 6; i++)
  {
    Serial.printf("Trying to send PID%d support, please turn on the car electronics\r\n", i);
    check_receive_pid = false;

    while(!check_receive_pid)
    {
      if(i==1)
      {
        Data[2] = PIDs1;
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      if(i==2)
      {
        Data[2] = PIDs2;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      if(i==3)
      {
        Data[2] = PIDs3;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      if(i==4)
      {
        Data[2] = PIDs4;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      if(i==5)
      {
        Data[2] = PIDs5;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      vTaskDelay(1);
    }

    if((i+1)==6) break;
  }

  return false;
}

/*=========================== ISRs ====================================*/
void ticker200mHzISR()
{ 
  //insert(FueL_Status_PID);
  insert(Fuel_Pressure_PID);
  insert(Fuel_Level_PID);
  insert(Engine_CoolantP_ID);
  insert(Engine_Oil_PID);
  insert(Ambient_Temp_PID);
}

void ticker300mHzISR()
{
  insert(GPS_ST);
  insert(Distance_on_MIL_PID);
  insert(Distance_Travel_PID);
  insert(Odometer_PID);
}

void ticker1HzISR()
{
  insert(MAP_sensor_PID);
  insert(Throttle_Pos_PID);
  insert(Engine_FuelRate_PID);
  insert(Run_Time_PID);
  insert(Engine_LoadP_ID);
}

void ticker2HzISR()
{
  insert(Speed_PID);
  insert(Engine_RPM_ID);
}

void ticker10HzISR()
{
  insert(Accelerometer_ST);
}
