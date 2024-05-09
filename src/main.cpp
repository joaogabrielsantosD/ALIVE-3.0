#include <Arduino.h>
/* Acquisition data librarie */
#include <AcquisitionData.h>
/* CAN libraries */
#include <can_defs.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

boolean flagCANInit = false;  // If false indicates that the CAN module was not initialized successfully
TaskHandle_t CANtask = NULL;  // BLEtask = NULL;

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
  
  set_wdt_timer();

  init_tickers();

  xTaskCreatePinnedToCore(AcquisitionStateMachine, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  //xTaskCreatePinnedToCore(BLElog, "BLEstatemachine", 4096, NULL, 4, &BLEtask, 1);
}

void loop() { reset_rtc_wdt(); /* Reset the wathdog timer */ }

void AcquisitionStateMachine(void* arg)
{
  int _id;
  uint32_t initialTime = 0;

  while(1)
  {
    if(flagCANInit)
    {
      _id = CircularBuffer_state();

      initialTime = millis();    
      while(!checkReceive() && _id>=0x03)
      {
        if(millis() - initialTime >= 2500) break;
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
