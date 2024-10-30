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

BLE_packet_t packet;
TaskHandle_t CANtask = NULL, Modulestask = NULL, BLEtask = NULL;

/* Taks */
void CANprocess_Task(void *arg);
void ModulesProcess_Task(void *arg);
void BLEsenderData(void *arg);

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nINICIANDO ALIVE 3.0\r\n");

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
  xTaskCreatePinnedToCore(BLEsenderData, "BLEstatemachine", 4096, NULL, 5, &BLEtask, 0);
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
void BLEsenderData(void *arg)
{
  for (;;)
  {
    if (BLE_connected())
      Send_BLE_msg(packet);

    vTaskDelay(MAX_BLE_DELAY);
  }
}
