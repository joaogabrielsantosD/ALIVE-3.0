#ifndef ESPNOW_H
#define ESPNOW_H

#include <esp_now.h>
#include <WiFi.h>

typedef struct DataStruct {  // Define a estrutura DataStruct para troca de informações
  float temperature;   
  float voltage;   
  float current;
} DataStruct;

bool start_ESPNOW(void);
bool DataReceived(void);
void printMessages(void);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

#endif
