#include <Arduino.h>
#include "wdt.h"
#include "can_defs.h"
#include "StateMachine.h"
#include "message.h"
#include "tickerISR.h"
#include "CAN_PIDs.h"

boolean flagCANInit = true;   //se false indica que o modulo CAN não foi inicializado com sucesso
state_t state;
uint32_t initialTime = 0;
TaskHandle_t CANtask = NULL, BLEtask = NULL;

/* State Machine Functions */
void logCAN(void *arg);
void BLElog(void *arg);

void setup()
{    
  Serial.begin(9600);
  Serial.println("INICIANDO ALIVE.");

  // if there was an error in the CAN it shows
  flagCANInit = can_setup();
  if(!flagCANInit)
  {
    Serial.println("CAN error!!!");
    esp_restart();
  }

  set_mask_filt();
  //pinMode(CAN_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canISR, FALLING);

  setup_ticker();

  xTaskCreatePinnedToCore(logCAN, "CANstatemachine", 4096, NULL, 5, &CANtask, 0);
  xTaskCreatePinnedToCore(BLElog, "BLEstatemachine", 2048, NULL, 4, &BLEtask, 1);

  setupWDT();
}

void loop() { reset_rtc_wdt(); /* Reset the wathdog timer */ }

void logCAN(void *arg)
{ 
  while(1)
  {
    if(flagCANInit)
    {
      state = CircularBuffer_state();

      initialTime = millis();    
      while(!checkReceive() && state!=IDLE_ST)
      {
        if(millis() - initialTime >= 4000)
        {
          break;
        }
        vTaskDelay(5);
      }
    }

    if(checkReceive())
    {
      trataMsgRecCAN(); //rotina q trata qndo uma msg chega via can
    }
  }

  vTaskDelay(1);
}

void BLElog(void *arg)
{
  for(;;)
  {
    Serial.println("BLE state\n");
    vTaskDelay(1000);
  }
}
