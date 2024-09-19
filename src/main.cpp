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

boolean flagCANInit = false; // If false, indicate that the CAN module was not initialized successfully
TaskHandle_t CANtask = NULL, BLEtask = NULL;

void CANprocessTask(void *arg);
void BLEsenderData(void *arg);

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nINICIANDO ALIVE 3.0\r\n");

  start_CAN_device();

  set_wdt_timer();

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
  int circularbuffer_State = IDLE_ST;

  bool CanIDtype = TestIF_StdExt();
  Serial.println(CanIDtype);
  checkPID(CanIDtype);
  init_tickers();

  while (1)
  {
    circularbuffer_State = CircularBuffer_state(); 
    send_OBDmsg(circularbuffer_State, CanIDtype);
    vTaskDelay(10);
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
