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


typedef enum {IDLE_ST, DistanceTraveled_ST, EngineRPM_ST, VehicleSpeed_ST, FuelLevel_ST,EngineCoolant_ST} state_t;



/* ESP Tools */
/*
CircularBuffer<state_t, BUFFER_SIZE> state_buffer;
state_t current_state = IDLE_ST;
*/

CircularBuffer<int, BUFFER_SIZE> state_buffer;
int current_state = IDLE_ST;
bool t = false;


Ticker ticker1Hz;
Ticker ticker10Hz;
Ticker ticker20Hz; 

uint32_t initialTime = 0;

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

  
  ticker1Hz.attach(1, PIDs_1hz);
  ticker10Hz.attach(2, PIDs_10hz);
  ticker20Hz.attach(20, PIDs_20hz);
}

void loop() {
   if (flagCANInit == true){
      CircularBuffer_state();

      initialTime = millis();
      
      while(flagRecv == false && current_state!= IDLE_ST)
      {
        if(millis() - initialTime >= 5000)
        {
          break;
        }
      }
      
  }
  
  if (flagRecv == true){
    trataMsgRecCAN(); //rotina q trata qndo uma msg chega via can
  
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

void CircularBuffer_state(){
 // Serial.println("Entrou no CircularBuffer");
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
    //Serial.println(current_state);

    /*
  if(!t)
  {
    while(state_buffer.first() == 0) { state_buffer.pop(); }
    current_state = state_buffer.pop();
    
  }
    */
  switch(current_state) {
     
     //Serial.println("Entrou no Switchcase");
    case IDLE_ST:
      //Serial.println("i");
      break;
    
    case DistanceTraveled_ST:{   
        //Serial.println("Entrou no um");
      unsigned char messageData[8] = {0x02, 0x01, DistanceTraveled_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

      if(CAN.sendMsgBuf(CAN_ID, 1, 8, messageData) == CAN_OK){  
          //tempoinicial2 = millis();
      Serial.print("Send to CAN: id ");
      Serial.print(CAN_ID, HEX);
      Serial.print("  ");    
      
      for (int i = 0; i < 8; i++){
    
          Serial.print((messageData[i]),HEX); 
          Serial.print("\t");
      }
      /*
      if(!t)
      {
        current_state = IDLE_ST;
        t = true;
      }
      */
      Serial.println();
        
        
      }
      
      break;
    }
    case EngineRPM_ST:{
      //Serial.println("Entrou no 2");
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
      //Serial.println("Entrou no 3");

      unsigned char messageData[8] = {0x02, 0x01, VehicleSpeed_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

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
    case FuelLevel_ST:{
         // Serial.println("Entrou no 4");
      unsigned char messageData[8] = {0x02, 0x01, FuelLevel_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

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
    case EngineCoolant_ST:{
        //Serial.println("Entrou no 5");
      unsigned char messageData[8] = {0x02, 0x01, EngineCoolant_PID, 0x00, 0x00, 0x00, 0x00, 0x00};

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
  }
}


void MsgRecCAN(){
    flagRecv = true; //flag que indica que uma msg foi recebida via CAN

    //current_state = (!state_buffer.isEmpty() || state_buffer.isFull() ? state_buffer.pop() : IDLE_ST);
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

      if (messageData[2] == 49) {
        float A;
        float B;
        A = messageData[3] ;
        B = messageData[4] ;
        float Distance_Traveled = ((A*256)+B);
        Serial.print("Distance_Traveled:   ");
        Serial.println(Distance_Traveled);
      }


      if (messageData[2] == 12) {
        float RPM1;
        float RPM2;
        RPM1 = messageData[3] ;
        RPM2 = messageData[4] ;
        float ENGINE_RPM = ((RPM1*256)+RPM2)/4;
        Serial.print("ENGINE_RPM:   ");
        Serial.println(ENGINE_RPM);
      }

      if (messageData[2] == 13) {
        float A;
        A = messageData[3];        
        float Vehicle_Speed = A;
        Serial.print("Vehicle_Speed:   ");
        Serial.println(Vehicle_Speed);
      }

      if (messageData[2] == 47) {
        float A;
        A = messageData[3];        
        float FuelLevel = (100*A)/255;
        Serial.print("FuelLevel:   ");
        Serial.println(FuelLevel);
      }

      if (messageData[2] == 5) {
        float A;
        A = messageData[3];        
        float EngineCollant = A-40;
        Serial.print("EngineCollant:   ");
        Serial.println(EngineCollant);
      }

  }
}

void PIDs_1hz(){
 
  state_buffer.push(DistanceTraveled_ST);
  
  state_buffer.push(EngineRPM_ST);
  

}

void PIDs_10hz(){
  
  state_buffer.push(VehicleSpeed_ST);
  state_buffer.push(FuelLevel_ST); 
  state_buffer.push(EngineCoolant_ST);

}


void PIDs_20hz(){

}


