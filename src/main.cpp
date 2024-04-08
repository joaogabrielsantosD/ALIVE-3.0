#include <Arduino.h>
//#include <Ticker.h>
#include "can_defs.h"
#include "StateMachine.h"
#include "message.h"
#include "tickerISR.h"
#include "CAN_PIDs.h"

boolean flagCANInit = true;   //se false indica que o modulo CAN não foi inicializado com sucesso
state_t state;
uint32_t initialTime = 0;

/* State Machine Functions */
//void logCAN(void *pvParameters);
//void BLElog(void *pvParameters);

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

  //xTaskCreatePinnedToCore(logCAN, "CANloggingStateMachine", 10000, NULL, 5, NULL, 0);
 // xTaskCreatePinnedToCore(BLElog, "BluetoothStateMachine", 10000, NULL, 5, NULL, 1);
}

void loop() {
  if(flagCANInit)
    {
     state = CircularBuffer_state();

      initialTime = millis();    
      while(!checkReceive() && state!=IDLE_ST)
      {
        if(millis() - initialTime >= 5000)
        {
          break;
        }
      }
    }

    if(checkReceive())
    {
      trataMsgRecCAN(); //rotina q trata qndo uma msg chega via can
    }

}

//void logCAN(void *pvParameters)
//{
//  while(1)
//  {
//    
//    vTaskDelay(1);
//  }
//}
/*
void BLElog(void *pvParameters)
{
  while(1)
  {

    vTaskDelay(100);
  }
}

*/