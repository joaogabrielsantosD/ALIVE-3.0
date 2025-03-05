#ifndef BLE_H
#define BLE_H

#pragma once

#include <BLEDevice.h>
#include <ArduinoJson.h>
#include "CanFunctions.h"
#include "tickerISR.h"

#define BLE_DEBUG_LED       GPIO_NUM_26 // Pin to debug led of BLE communication 

#define SERVICE_UUID        "acc1f4ef-4fcf-4f90-882e-0a666da9f321"
#define CHARACTERISTIC_UUID "50e6cbc6-5aff-4423-974c-0e27959453c3"

#define MAX_BLE_LENGTH       ESP_GATT_MAX_ATTR_LEN // 600 bytes
#define MAX_BLE_DELAY        90
#define DOC_SIZE_JSON        400

class BLEHandler : public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
    BLEServer *pServer = NULL;
    BLEService *pService = NULL;
    BLECharacteristic *pCharacteristic = NULL;
    // BLEDescriptor *pDescr_1 = NULL;
    // BLE2902 *pBLE2902_1 = NULL;

    bool deviceConnected = false, oldDeviceConnected = false;
    std::string msgBLE = "";
public:
    BLEHandler() = default;

    /**
    * @brief Initializes the BLE server.
    * @note Sets up the BLE server, configures services and characteristics, 
    *       and starts advertising for connections.
    */
    void Init_BLE_Server(void);

    /**
    * @brief Checks and manages BLE connection status.
    * @note Handles device connection and disconnection events, 
    *       updates LED status, and restarts advertising when needed.
    * @return 
    *   -   True if a device is connected 
    *   -   false otherwise.
    */    
    bool BLE_connected(void);

    /**
    * @brief Sends a BLE message.
    * @note Sends a BLE message to the connected device in format JSON.
    * @param msg_packet The BLE message to be sent.
    */
    void Send_BLE_msg(BLE_packet_t msg_packet);

protected: // Callback to receive device connection events
    /**
    * @brief Callback to receive device connection events.
    */    
    void onConnect(BLEServer *pServer) override;
    
    /**
    * @brief Callback to receive device disconnection events.
    */
    void onDisconnect(BLEServer *pServer) override;

protected: // Callback to receive characteristic write events
    /**
    * @brief Callback to receive characteristic write events.
    */
    // friend void onWrite(BLECharacteristic *SenderCharacteristic) override;
};

extern BLEHandler BLE;

#endif
