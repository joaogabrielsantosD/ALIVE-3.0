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
  // start_module_device();

  /* Create the task responsible to the Acquisition(CAN + Accelerometer + GPS) and Connectivity(BLE) management */
  xTaskCreatePinnedToCore(CANprocessTask, "CANstatemachine", 4096, NULL, 3, &CANtask, 1);
  xTaskCreatePinnedToCore(BLEsenderData, "BLEstatemachine", 4096, NULL, 2, &BLEtask, 0);
}

void loop() { reset_rtc_wdt(); }

void CANprocessTask(void *arg)
{
  static bool run_time_once = false;
  int circularbuffer_State = IDLE_ST;
  uint8_t CanIDtype = TestIF_StdExt();

  if (CanIDtype != 2 && !run_time_once)
  {
    checkPID();
    init_tickers();
    run_time_once = true;
  }

  while (1)
  {
    if (CanIDtype == 2)
      CanIDtype = TestIF_StdExt();

    if (CanIDtype != 2 && !run_time_once)
    {
      checkPID();
      init_tickers();
      run_time_once = true;
    }

    if (CanIDtype < 2 && run_time_once)
    {
      circularbuffer_State = CircularBuffer_state();
      if (circularbuffer_State != 0)
      {
        Serial.printf("\r\n Current_PID is %d", circularbuffer_State);
        send_OBDmsg(circularbuffer_State);
      }
    }

    vTaskDelay(CanIDtype == 2 ? 1000 : 1);
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
