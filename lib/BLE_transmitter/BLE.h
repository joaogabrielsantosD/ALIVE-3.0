#ifndef BLE_H
#define BLE_H

#include <BLEDevice.h>
#include <ArduinoJson.h>
#include "CircularBufferState.h"

#define SERVICE_UUID        "acc1f4ef-4fcf-4f90-882e-0a666da9f321"
#define CHARACTERISTIC_UUID "50e6cbc6-5aff-4423-974c-0e27959453c3"

#define MAX_BLE_LENGTH       512
#define MAX_BLE_DELAY        100 // ms
#define DOC_SIZE_JSON        400

void Init_BLE_Server(void);
bool BLE_connected(void);
void Send_BLE_msg(BLE_packet_t msg_packet);
void Make_JSON_packet(StaticJsonDocument<DOC_SIZE_JSON> &JSON, BLE_packet_t &msg_packet);

// Callback to receive device connection events
class ServerCallbacks : public BLEServerCallbacks
{
    public:
        void onConnect(BLEServer *pServer);

        void onDisconnect(BLEServer *pServer);
};

#endif
