#include <Arduino.h>
/* Acquisition data library */
#include <AcquisitionData.h>
/* BLE sender data library */
#include <BLE.h>
/* CAN libraries */
#include <CanFunctions.h>
/* State Machine and Bit analyze librarie */
#include <CircularBufferState.h>
/* Ticker interrupts librarie */
#include <tickerISR.h>
/* WatchDog timer libraries */
#include <wdt.h>

#include <esp_now.h>            // Biblioteca para utilizar o protocolo de comunicação ESP-NOW
#include <WiFi.h>               // Biblioteca para conectar em redes Wi-Fi

BLE_packet_t packet;
TaskHandle_t CANtask = NULL, Modulestask = NULL, BLEtask = NULL;

/* Tasks */
void CANprocess_Task(void *arg);
void ModulesProcess_Task(void *arg);
void BLEsenderData_Task(void *arg);
void TaskESPNow(void *pvParameters);

typedef struct DataStruct {  // Define a estrutura DataStruct para troca de informações
  float temperature;   
  float voltage;   
  float current;
} DataStruct;

DataStruct message;
bool newDataReceived = false;  // Flag para indicar novos dados recebidos

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&message, incomingData, sizeof(message));

  //Serial.print("Bytes received: ");
  //Serial.println(len);
  newDataReceived = true; 
  
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nINICIANDO ALIVE 3.0\r\n");

  
  // Desconecta de alguma conexão WiFi anterior e define o modo estação (STA)
 WiFi.disconnect();
 WiFi.mode(WIFI_STA);
 Serial.print("Endereço MAC: ");
 Serial.println(WiFi.macAddress()); // retorna o endereço MAC do dispositivo

 // Inicia a biblioteca ESP-NOW e, caso ocorra algum erro, reinicia o dispositivo
 if (esp_now_init() != ESP_OK) {    
   Serial.print("ESP-NOW com Erro");
   //ESP.restart();
 }
 // Registra a função OnDataRecv como a função a ser chamada quando receber dados via ESP-NOW
  esp_now_register_recv_cb(OnDataRecv);

  /* Set all package values to 0 */
  memset(&packet, 0, sizeof(BLE_packet_t));
  packet.DTC = "null";

  /* Start the MCP2515 to CAN communication */
  start_CAN_device();

  /* Set the new WDT timer */
  set_wdt_timer();

  /* Init the BLE host connection */
  Init_BLE_Server();

  /* Init the Modules */
  start_module_device();

  /* Create the task responsible to the Acquisition(CAN + Accelerometer + GPS) */
  xTaskCreatePinnedToCore(CANprocess_Task, "CANstatemachine", 2048, NULL, 4, &CANtask, 1);
  xTaskCreatePinnedToCore(ModulesProcess_Task, "Modulesstatemachine", 2048, NULL, 3, &Modulestask, 1);

  /* Create the task responsible to the Connectivity(BLE + ESPNOW) management */
  xTaskCreatePinnedToCore(BLEsenderData_Task, "BLEstatemachine", 4096, NULL, 1, &BLEtask, 0);

  xTaskCreatePinnedToCore(TaskESPNow, "ESPNow Task", 4096, NULL, 1, NULL, 0);
}

void loop() { reset_rtc_wdt(); }

/* Core 1: Acquisition Threads */
void CANprocess_Task(void *arg)
{
  static int circularbuffer_State = IDLE_ST;

  TestIF_StdExt();
  checkPID();
  init_tickers();

  while (1)
  {
    circularbuffer_State = CircularBuffer_state();

    if (circularbuffer_State != IDLE_ST)
      send_OBDmsg(circularbuffer_State, &packet);

    vTaskDelay(1);
  }
}

void ModulesProcess_Task(void *arg)
{
  static uint8_t gps_counter_per_seconds = 0;     // Each second will be incremented
  const static uint8_t Time_to_get_gps_data = 30; // Expected time to get/update the gps data (in seconds)

  while (1)
  {
    gps_counter_per_seconds++;

    if (gps_counter_per_seconds == Time_to_get_gps_data)
    {
      gps_acq_function(&packet);
      gps_counter_per_seconds = 0;
    }

    imu_acq_function(&packet);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/* Core 0: Telemetry Threads */
void BLEsenderData_Task(void *arg)
{
  for (;;)
  {
    if (BLE_connected())
      Send_BLE_msg(packet);

    vTaskDelay(MAX_BLE_DELAY);
  }
}

// Tarefa FreeRTOS para lidar com a recepção de dados via ESP-NOW
void TaskESPNow(void *pvParameters) {
  for (;;) {
    //Serial.println("TASK ESPNOW");
    //Serial.printf("ESPNOW received?: %d",newDataReceived);

    if(newDataReceived) {    
     
      Serial.print("Temperature: ");
      Serial.println(message.temperature);
      Serial.print("Voltage: ");
      Serial.println(message.voltage);
      newDataReceived = false;  // Reseta a flag
      // Processamento adicional dos dados recebidos, se necessário
      
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Pequeno delay para não ocupar toda a CPU
  }
}
