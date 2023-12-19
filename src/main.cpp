#include <Arduino.h>
#include <SPI.h>
#include <mcp2515_can.h>

#define CAN_2515
const int SPI_CS_PIN = 5;
const int CAN_INT_PIN = 27; // Pino usado para gerar a interrupção pelo modulo MCP2515
#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
//#define MAX_DATA_SIZE 8
#endif


boolean flagCANInit = true; //se false indica que o modulo CAN não foi inicializado com sucesso

unsigned int idMsgCANRec = 0; //essa variavel armazena o id da msg CAN q enviou uma msg CAN

boolean flagRecv = false; //se true indica q uma msg foi recebida via CAN


void setup()
{    
unsigned long tcanStart = 0, cantimeOut = 0;
  tcanStart = millis();
  cantimeOut = 1000; // (1 second)
  // wait for the CAN shield to initialize

  Serial.println("Connecting CAN...");
  while((millis() - tcanStart) < cantimeOut) // wait timeout
  { 
    if(CAN.begin(CAN_1000KBPS, MCP_8MHz) == CAN_OK)
    {
      Serial.println("CAN init ok!!!");
      flagCANInit = true; // Marks the flag that indicates correct CAN initialization
      break; // get out of the loop
    }
    flagCANInit = false; // Mark the flag that indicates there was a problem initializing the CAN
  }

  // if there was an error in the CAN it shows
  if(!flagCANInit)
  {
    Serial.println("CAN error!!!");
    esp_restart();
  }
}

void loop() {
  
  canReceiver();

}

void canReceiver(){
  
  while(CAN.checkReceive() == CAN_MSGAVAIL)
  {
    //Serial.println("ok!");
    byte messageData[8];
    uint32_t messageId;
    unsigned char len = 0; //armazena o tamanho da msg CAN (qtd de dados recebidos)

    // Reads message and ID
    CAN.readMsgBuf(&len, messageData); 
    messageId = CAN.getCanId();

      Serial.print("Recieve by CAN: id ");

      Serial.print(messageId);
      Serial.print(" (");
      Serial.print(messageId, HEX);
      Serial.print(")HEX");
      Serial.print("\t");
      // print the data
      
      for (int i = 0; i < len; i++){
    
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
      }
      
      Serial.println();
  }
}