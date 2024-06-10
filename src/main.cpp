#include <Arduino.h>
/* Acquisition data librarie */
#include <AcquisitionData.h>
/* BLE sender data librarie */
#include <BLE.h> 
/* CAN libraries */
#include <can_defs.h>
/* State Machine and Bit analyze librarie */
#include <StateMachine.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

boolean flagCANInit = false;  // If false indicates that the CAN module was not initialized successfully
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
  if(!flagCANInit)
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
  int _canId = 0;
  unsigned long initialTime = 0;

  while(1)
  {
    if(flagCANInit)
    {
      _canId = CircularBuffer_state(); // check if the current id is CAN message or not

      if(_canId == 1 || _canId == 2)
      {
        /* 0x01 is ACC data and 0x02 is GPS data, the acq_function(int acq_mode) 
         * call the threads will be execute the acquisition function of respective data.
         * The @param acq_mode only accept 0x01 or 0x02, bigger then this
         * will be ignored */
        acq_function(_canId);
      } else {
        initialTime = millis();    
        while(!checkReceive() && _canId > 0x02)
        { // timeout
          if(millis() - initialTime >= 3000) break;
          vTaskDelay(1);
        }
      }
    }

    if(checkReceive() && flagCANInit)
    {
      // Routine that handles when a message arrives via can
      MsgRec_Treatment(); 
    }
    
    vTaskDelay(1);
  }
}

void BLEsenderData(void *arg)
{
  for(;;)
  {
    while(BLE_connected())
    {
      Send_BLE_msg();

      vTaskDelay(MAX_BLE_DELAY + 10);
    }

    vTaskDelay(10);
  }
}
