#include <Arduino.h>
#include <SPI.h>
#include <mcp2515_can.h>
#include <CircularBuffer.hpp>
#include <Ticker.h>
#include "CAN_PIDs.h"

#define CAN_2515
const int SPI_CS_PIN = 5;
const int CAN_INT_PIN = 27; // Pino usado para gerar a interrupção pelo modulo MCP2515
#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
//#define MAX_DATA_SIZE 8
#endif

#define CAN_ID  0x18DB33F1

boolean flagCANInit = true; //se false indica que o modulo CAN não foi inicializado com sucesso

unsigned int idMsgCANRec = 0; //essa variavel armazena o id da msg CAN q enviou uma msg CAN

boolean flagRecv = false; //se true indica q uma msg foi recebida via CAN


/* Interrupt services routine */
//void canSender();
//void canReceiver();
void set_mask_filt();
void CircularBuffer_state();
void MsgRecCAN();
void trataMsgRecCAN();


/* CicularBuffer Defs: */

/*
const int bufferSize = 10;
CircularBuffer<char, bufferSize> state_buffer;
char current_state = 'IDLE_ST';
*/

typedef enum {IDLE_ST, DistanceTraveled_ST, EngineRPM_ST, VehicleSpeed_ST, FuelLevel_ST,EngineCoolant_ST} state_t;

/* ESP Tools */

CircularBuffer<state_t, BUFFER_SIZE> state_buffer;
state_t current_state = IDLE_ST;
Ticker ticker1Hz; 



void PIDs_1hz();
void PIDs_10hz();
void PIDs_20hz();

//Ticker ticker_1Hz(PIDs_1hz, 1000, 1); //1 time, every second
//Ticker ticker_10Hz(PIDs_10hz, 1000, 10);
//Ticker ticker_20Hz(PIDs_20hz, 1000, 20);

/* Debug Variables */
bool buffer_full = false;

void setup()
{    
  Serial.begin(9600);
  Serial.println("INICIANDO ALIVE.");
   
  /* Inicia a can: */

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
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), MsgRecCAN, FALLING);
/*
  ticker_1Hz.start();
  ticker_10Hz.start();
  ticker_20Hz.start();
*/
  
  ticker1Hz.attach(1, PIDs_1hz);
}

void loop() {

   if (flagCANInit == true){
      CircularBuffer_state();
      delay(1000);
      //canReceiver();
      //delay(1000);
  }

  if (flagRecv == true){
    trataMsgRecCAN(); //rotina q trata qndo uma msg chega via can
  } 

}

/*
void canReceiver()
{
 Serial.println("Entrou can receive"); 
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
*/
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

void CircularBuffer_state(){
  //Serial.println("Entrou no CircularBuffer");
  if(state_buffer.isFull())
  {
    buffer_full = true;
    current_state = state_buffer.pop();

  } else {
      buffer_full = false;
      if(!state_buffer.isEmpty())
        current_state = state_buffer.pop();
      else
        current_state = IDLE_ST;
  }

  switch(current_state) {
     // Serial.println("Entrou no Switchcase");
    case IDLE_ST:
      //Serial.println("i");
      break;
    /*
    case DistanceTraveled_ST:{   

      unsigned char messageData[8] = {0x02, 0x01, DistanceTraveled_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(CAN.sendMsgBuf(CAN_ID, 1, 8, messageData) == CAN_OK){  

        Serial.print("Send to CAN: id ");
        Serial.print(CAN_ID, HEX);
        Serial.print("  ");    
        
        for (int i = 0; i < 8; i++){
      
            Serial.print((messageData[i]),HEX); 
            Serial.print("\t");
        }
        
        Serial.println();
      }
    
      break;
    }
    */
    case EngineRPM_ST:{

      unsigned char messageData[8] = {0x02, 0x01, EngineRPM_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(CAN.sendMsgBuf(CAN_ID, 1, 8, messageData) == CAN_OK) {  

          Serial.print("Send to CAN: id ");
          Serial.print(CAN_ID, HEX);
          Serial.print("  ");    
          
          for (int i = 0; i < 8; i++){
        
              Serial.print((messageData[i]),HEX); 
              Serial.print("\t");
          }
          
          Serial.println();
        }
        
        break;
    }
    case VehicleSpeed_ST:{
      
      break;
    }
  }
}


void MsgRecCAN(){
    flagRecv = true; //flag que indica que uma msg foi recebida via CAN
    //Serial.println("teste direc");
}

void trataMsgRecCAN(){
   flagRecv = false; // clear flag


  
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
void PIDs_1hz(){
  //state_buffer.push(DistanceTraveled_ST);
  state_buffer.push(EngineRPM_ST);
  //Serial.println("Funcionando ticker");
}
void PIDs_10hz(){

}
void PIDs_20hz(){

}

