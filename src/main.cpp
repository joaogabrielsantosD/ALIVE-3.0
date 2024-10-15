#include <Arduino.h>
/* Acquisition data library */
#include <AcquisitionData.h>
/* BLE sender data library */
#include <BLE.h>
/* CAN libraries */
#include <CanFunctions.h>
/* State Machine and Bit analyze librarie */
#include <CircularBufferState.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

TaskHandle_t CANtask = NULL, BLEtask = NULL;

void CANprocessTask(void *arg);
void BLEsenderData(void *arg);

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nINICIANDO ALIVE 3.0\r\n");

  /* Start the MCP2515 to CAN communication */
  start_CAN_device();

  /* Set the new WDT timer */
  set_wdt_timer();

  /* Init the BLE host connection */
  Init_BLE_Server();

  /* Init the Modules */
  start_module_device();

  /* Create the task responsible to the Acquisition(CAN + Accelerometer + GPS) and Connectivity(BLE) management */
  xTaskCreatePinnedToCore(CANprocessTask, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  xTaskCreatePinnedToCore(BLEsenderData, "BLEstatemachine", 4096, NULL, 4, &BLEtask, 1);
}

void loop() { reset_rtc_wdt(); }

void CANprocessTask(void *arg)
{
  static bool run_once_time = false;
  int circularbuffer_State = IDLE_ST;

  uint8_t can_flag = TestIF_StdExt();
  if (can_flag != 2 && !run_once_time)
  {
    checkPID();
    init_tickers();
    run_once_time = true;
  }

  while (1)
  {
    if (can_flag == 2)
      can_flag = TestIF_StdExt();

    if (can_flag != 2 && !run_once_time)
    {
      checkPID();
      init_tickers();
      run_once_time = true;
    }

    if ((can_flag == 0 || can_flag == 1) && run_once_time)
    {
      circularbuffer_State = CircularBuffer_state();
      Serial.printf("\r\n Current_PID is %d", circularbuffer_State);
      send_OBDmsg(circularbuffer_State);
    }

    vTaskDelay(can_flag == 2 ? 1000 : 1);
  }
}

void BLEsenderData(void *arg)
{
  for (;;)
  {
    while (BLE_connected())
    {
      Send_BLE_msg();

      vTaskDelay(MAX_BLE_DELAY + 10);
    }

    vTaskDelay(10);
  }
}
