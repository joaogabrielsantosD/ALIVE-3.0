#include <ESPNOW.h>

DataStruct message;
bool newDataReceived = false;  // Flag para indicar novos dados recebidos

bool start_ESPNOW()
{
    // Desconecta de alguma conexão WiFi anterior e define o modo estação (STA)
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    Serial.print("Endereço MAC: ");
    Serial.println(WiFi.macAddress()); // retorna o endereço MAC do dispositivo

    // Inicia a biblioteca ESP-NOW e, caso ocorra algum erro, reinicia o dispositivo
    if (esp_now_init() != ESP_OK) 
    {    
        Serial.print("ESP-NOW com Erro");
        //ESP.restart();
        return false;
    } 
    
    // Registra a função OnDataRecv como a função a ser chamada quando receber dados via ESP-NOW
    if (esp_now_register_recv_cb(OnDataRecv) != ESP_OK)
        return false;
    
    return true;
}

bool DataReceived()
{
    return newDataReceived;
}

void printMessages()
{
    Serial.print("Temperature: ");
    Serial.println(message.temperature);
    Serial.print("Voltage: ");
    Serial.println(message.voltage);
    newDataReceived = false;  // Reseta a flag
    // Processamento adicional dos dados recebidos, se necessário
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) 
{
  memcpy(&message, (DataStruct*)incomingData, sizeof(message));

  //Serial.print("Bytes received: ");
  //Serial.println(len);
  newDataReceived = true;   
}
