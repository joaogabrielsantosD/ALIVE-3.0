#include <Arduino.h>
/* Acquisition data librarie */
#include <AcquisitionData.h>
/* BLE sender data librarie */
#include <BLE.h>
/* CAN libraries */
#include <can_defs.h>
/* State Machine and Bit analyze librarie */
#include <CircularBufferState.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

boolean flagCANInit = false; // If false indicates that the CAN module was not initialized successfully
TaskHandle_t CANtask = NULL, BLEtask = NULL;

/* State Machine Functions */
void AcquisitionStateMachine(void *arg);
void BLEsenderData(void *arg);

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nINICIANDO ALIVE 3.0\r\n");

  // if there was an error in the CAN it shows
  flagCANInit = start_CAN_device(true);
  if (!flagCANInit)
  {
    Serial.println("CAN error!!!");    
    esp_restart();
  }

  /* Disable the WDT */
  set_wdt_timer();

  /* Initialize all tickers to insert the messages in the circular buffer */
  init_tickers();

  /* Set the new WDT timer */
  set_wdt_timer();

  /* Init the BLE host connection */
  Init_BLE_Server();

  /* Init the Modules */
  start_module_device();

  /* Create the task responsible to the Acquisition(CAN + Accelerometer + GPS) and Connectivity(BLE) management */
  xTaskCreatePinnedToCore(AcquisitionStateMachine, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  xTaskCreatePinnedToCore(BLEsenderData, "BLEstatemachine", 4096, NULL, 4, &BLEtask, 1);
}

void loop() { reset_rtc_wdt(); /* Reset the wathdog timer */ }

void AcquisitionStateMachine(void *arg)
{
  int circularbuffer_State = IDLE_ST;

  while (1)
  {
    if (flagCANInit)
    {
      circularbuffer_State = CircularBuffer_state(); // check if the current id is CAN message or not

      acq_function(circularbuffer_State);
    }

    vTaskDelay(1);
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
