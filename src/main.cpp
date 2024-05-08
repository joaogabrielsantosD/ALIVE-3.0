#include <Arduino.h>
#include "wdt.h"
#include "can_defs.h"

boolean flagCANInit = false;   // If false indicates that the CAN module was not initialized successfully
TaskHandle_t CANtask = NULL;// BLEtask = NULL;

/* State Machine Functions */
void logCAN(void* arg);
//void BLElog(void* arg);

void setup()
{    
  Serial.begin(9600);
  //vTaskDelay(1000); // delay to avoid the bug in the serialprint when you use the monitor serial to reset the module
  Serial.println("INICIANDO ALIVE.");
  
  pinMode(LED_BUILTIN, OUTPUT);

  // if there was an error in the CAN it shows
  flagCANInit = can_setup();
  if(!flagCANInit)
  {
    Serial.println("CAN error!!!");
    esp_restart();
  }

  set_mask_filt();
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canISR, FALLING);

  //xTaskCreatePinnedToCore(logCAN, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  //xTaskCreatePinnedToCore(BLElog, "BLEstatemachine", 4096, NULL, 4, &BLEtask, 1);

  setupWDT();
}

void loop() { reset_rtc_wdt(); /* Reset the wathdog timer */ }

/*void logCAN(void* arg)
{
  uint32_t initialTime = 0;

  while(1)
  {
    if(flagCANInit)
    {
      _id = CircularBuffer_state();

      initialTime = millis();    
      while(!checkReceive() && _id!=0)
      {
        if(millis() - initialTime >= 4000) break;
        vTaskDelay(1);
      }
    }

    if(checkReceive() && flagCANInit)
    {
      // Routine that handles when a message arrives via can
      MsgRec_CANroutine(); 
    }
  }

  vTaskDelay(1);
}*/
