#include <Arduino.h>
/* Acquisition data librarie */
#include <AcquisitionData.h>
/* CAN libraries */
#include <can_defs.h>
/* State Machine and Bit analyze librarie */
#include <StateMachine.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

boolean flagCANInit = false;  // If false indicates that the CAN module was not initialized successfully
TaskHandle_t CANtask = NULL;

/* State Machine Functions */
void AcquisitionStateMachine(void* arg);
//void BLElog(void* arg);

void setup()
{    
  Serial.begin(9600);
  //vTaskDelay(1000); // delay to avoid the bug in the serialprint when you use the monitor serial to reset the module
  Serial.println("INICIANDO ALIVE 3.0");
  
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
  /* Create the task responsible to the Acquisition(CAN + Accelerometer + GPS) and Connectivity(WiFi + OTA) management */
  xTaskCreatePinnedToCore(AcquisitionStateMachine, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  //xTaskCreatePinnedToCore(BLElog, "BLEstatemachine", 4096, NULL, 4, &BLEtask, 1);
}

void loop() { reset_rtc_wdt(); /* Reset the wathdog timer */ }

void AcquisitionStateMachine(void* arg)
{
  int _canId = 0;
  unsigned long initialTime = 0;

  while(1)
  {
    if(flagCANInit)
    {
      _canId = CircularBuffer_state(); // check if the current id is CAN message or not

      initialTime = millis();    
      while(!checkReceive() && _canId!=0)
      { // timeout
        if(millis() - initialTime >= 3000) break;
        vTaskDelay(1);
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
