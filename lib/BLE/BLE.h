#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define MAX_BLE_LENGTH ESP_GATT_MAX_ATTR_LEN
#define MAX_BLE_DELAY  90

void setup_BLE(void);
int BLE_connected(void);
void BLE_Sender(void* T, uint32_t len);

// Callback para eventos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks 
{
    public:
        void onWrite(BLECharacteristic* characteristic); 
};

// Callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks 
{
    public: 
        void onConnect(BLEServer* pServer);

        void onDisconnect(BLEServer* pServer);
};

#endif
