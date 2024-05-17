#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <ArduinoJson.h>
#include "AcquisitionData.h"
#include "ALIVE_defs.h"

#define SERVICE_UUID          "acc1f4ef-4fcf-4f90-882e-0a666da9f321"
#define CHARACTERISTIC_UUID   "50e6cbc6-5aff-4423-974c-0e27959453c3"

#define MAX_BLE_LENGTH        ESP_GATT_MAX_ATTR_LEN
#define MAX_BLE_DELAY         90
#define DOC_SIZE_JSON         300

void setup_BLE(void);
int BLE_connected(void);
void Send_BLE_msg(void);
void BLE_sender(StaticJsonDocument<DOC_SIZE_JSON>& document);

// Callback to receive device connection events
class ServerCallbacks: public BLEServerCallbacks 
{
    public: 
        void onConnect(BLEServer* pServer);

        void onDisconnect(BLEServer* pServer);
};

#endif