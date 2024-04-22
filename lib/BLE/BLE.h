#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID          "acc1f4ef-4fcf-4f90-882e-0a666da9f321"
#define CHARACTERISTIC_UUID   "50e6cbc6-5aff-4423-974c-0e27959453c3"

#define MAX_BLE_LENGTH ESP_GATT_MAX_ATTR_LEN
#define MAX_BLE_DELAY  45*2

void setup_BLE(void);
int BLE_connected(void);
void BLE_Sender(void* T, uint32_t len);

// Callback to receive device connection events
class ServerCallbacks: public BLEServerCallbacks 
{
    public: 
        void onConnect(BLEServer* pServer);

        void onDisconnect(BLEServer* pServer);
};

// Callback for feature events
class CharacteristicCallbacks: public BLECharacteristicCallbacks 
{
    public:
        void onWrite(BLECharacteristic* characteristic); 
};

#endif
