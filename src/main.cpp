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

#define CAN_ID_PID  0x18DB33F1

boolean flagCANInit = true; //se false indica que o modulo CAN não foi inicializado com sucesso

unsigned int idMsgCANRec = 0; //essa variavel armazena o id da msg CAN q enviou uma msg CAN

boolean flagRecv = false; //se true indica q uma msg foi recebida via CAN

void canSender();
void canReceiver();
void set_mask_filt();


void setup()
{    
  Serial.begin(9600);
  Serial.println("INICIANDO ALIVE.");

  unsigned long tcanStart = 0, cantimeOut = 0;
  tcanStart = millis();
  cantimeOut = 1000; // (1 second)
  // wait for the CAN shield to initialize

  Serial.println("Connecting CAN...");
  while((millis() - tcanStart) < cantimeOut) // wait timeout
  { 
    if(CAN.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK)
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

  set_mask_filt();
  //pinMode(CAN_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canReceiver, FALLING);
}

void loop() {
  if (flagCANInit == true){
      canSender();
      delay(1000);
      canReceiver();
      delay(1000);
  }
  

}

void canSender(){
  unsigned char messageId_obd2 = 0x18DB33F1;
  uint32_t identifier = 1;
  unsigned char messageData[8] = {0x02, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
  CAN.sendMsgBuf(CAN_ID_PID, 1, 8, messageData);

     Serial.print("Send to CAN: id ");
      Serial.print(CAN_ID_PID, HEX);
      Serial.print("  ");
      //Serial.print(messageId_obd2, HEX);
      //Serial.print(")HEX");
     // Serial.print("\t");
     // print the data
      
      for (int i = 0; i < 8; i++){
    
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
      }
      
      Serial.println();

}

void canReceiver()
{
  
  while(CAN.checkReceive() == CAN_MSGAVAIL)
  {
    //Serial.println("ok!");
    unsigned char messageData[8];
    uint32_t messageId;
    unsigned char len = 0; //armazena o tamanho da msg CAN (qtd de dados recebidos)

    // Reads message and ID
    CAN.readMsgBuf(&len, messageData); 
    messageId = CAN.getCanId();

      Serial.print("Recieve by CAN: id ");

      //Serial.print(messageId);
      //Serial.print(" (");
      Serial.print(messageId, HEX);
      //Serial.print(")HEX");
      Serial.print("\t");
      // print the data
      
      for (int i = 0; i < len; i++){
    
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
      }
      
      Serial.println();
      if (messageData[2] == 12) {
        float RPM1;
        float RPM2;
        RPM1 = messageData[3] ;
        RPM2 = messageData[4] ;
        float ENGINE_RPM = ((RPM1*256)+RPM2)/4;
        Serial.print("ENGINE_RPM:   ");
        Serial.println(ENGINE_RPM);
    }
  }
      
}

void set_mask_filt() {

    /*
        set mask, set both the mask to 0x3ff
    */
    CAN.init_Mask(0, 1, 0x1FFFFFFF);
    CAN.init_Mask(1, 1, 0x1FFFFFFF);

    /*
        set filter, we can receive id from 0x04 ~ 0x09
    */
    CAN.init_Filt(0, 1, 0x18DAF110);
    CAN.init_Filt(1, 1, 0x18DAF110);

    CAN.init_Filt(2, 1, 0x18DAF110);
    CAN.init_Filt(3, 1, 0x18DAF110);
    CAN.init_Filt(4, 1, 0x18DAF110);
    CAN.init_Filt(5, 1, 0x18DAF110);
}

