#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "message.h"

//#define SERVICE_UUID            "ab0828b1-198e-4351-b779-901fa0e0371e"
//#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
//#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define MAX_BLE_LENGTH 64
#define MAX_BLE_DELAY  90

void setup_BLE(void);
void BLE_connected(BLEmsg_t Q);
void BLE_Sender(void* T, int len);

//callback para eventos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks 
{
    public:
        void onWrite(BLECharacteristic *characteristic); 
};

//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks 
{
    public: 
        void onConnect(BLEServer* pServer);

        void onDisconnect(BLEServer* pServer);
};


#endif