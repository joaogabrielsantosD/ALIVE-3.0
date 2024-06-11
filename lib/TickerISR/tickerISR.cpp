#include "tickerISR.h"

bool Print_in_serial = true; // Put True or False to enable SerialPrint of checkPID
const unsigned char Pids[] = {PIDs1, PIDs2, PIDs3, PIDs4, PIDs5};
Ticker ticker200mHz, 
    ticker300mHz, 
    ticker1Hz,
    ticker2Hz;

void init_tickers()
{
  do 
  {
    Serial.println("Check the PID support...");
  } while(checkPID());
  
  ticker200mHz.attach(5.0f, ticker200mHzISR); // 5s
  ticker300mHz.attach(3.0f, ticker300mHzISR); // 3s
  ticker1Hz.attach(1.0f, ticker1HzISR);       // 1s
  ticker2Hz.attach(0.5f, ticker2HzISR);       // 0.5s
}

bool checkPID()
{
  bool extended = false;
  // Flag to check if you received the PID support message
  bool check_receive_pid = false;
  unsigned char Data[8] = {0x04, 0x01, 0x00/*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  for(int i = 0; i < sizeof(Pids); i++)
  {
    Serial.printf("Trying to send PID[%d] support, please turn on the car electronics\r\n", i + 1);
    check_receive_pid = false;
    Data[2] = Pids[i];

    while(!check_receive_pid)
    {
      if(i == 0)
      {
        unsigned long obd_connection = millis();
        const unsigned long OBD_timout = 3000; // 3 seconds
        while(!checkReceive())
        {
          if(send_msg(Data, extended) && Print_in_serial) 
            debug_print(Data);
          extended = !extended;

          // timeout for OBD II connection failed
          if((millis() - obd_connection) >= OBD_timout)
            esp_restart();
          
          vTaskDelay(10);          
        }
        SaveParameters_extended(!extended);
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      else
      {
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) 
            debug_print(Data);
          vTaskDelay(10);          
        }
        MsgRec_Treatment();
        check_receive_pid = true;
      }

      vTaskDelay(1);
    }
  }

  return false;
}

/*=========================== ISRs ====================================*/
void ticker200mHzISR()
{ 
  insert(Fuel_Level_PID);
  insert(Engine_CoolantP_ID);
  insert(Engine_Oil_PID);
  insert(Ambient_Temp_PID);
}

void ticker300mHzISR()
{
  //insert(FueL_Status_PID);
  insert(Odometer_PID);  
  insert(Distance_on_MIL_PID);
  insert(Distance_Travel_PID);
  insert(GPS_ST);
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
  insert(Accelerometer_ST);
  insert(Fuel_Pressure_PID);
  insert(Speed_PID);
  insert(Engine_RPM_ID);
}
