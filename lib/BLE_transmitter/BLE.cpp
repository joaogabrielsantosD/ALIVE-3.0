#include "BLE.h"

bool deviceConnected = false, oldDeviceConnected = false;
std::string msgBLE = "";
BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pCharacteristic = NULL;

void setup_BLE()
{
    // Create the BLE Device
    BLEDevice::init("Dongle OBD");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    BLEService* pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(                                    \
                                                    CHARACTERISTIC_UUID,                 \
                                                    BLECharacteristic::PROPERTY_WRITE  | \
                                                    BLECharacteristic::PROPERTY_NOTIFY   \
                                                    );

  //// Create a BLE Descriptor  
  //pDescr_1 = new BLEDescriptor((uint16_t)0x2901);
  //pDescr_1->setValue("A very interesting variable");
  //pCharacteristic_1->addDescriptor(pDescr_1);

  //// Add the BLE2902 Descriptor because we are using "PROPERTY_NOTIFY"
  //pBLE2902_1 = new BLE2902();
  //pBLE2902_1->setNotifications(true);                 
  //pCharacteristic_1->addDescriptor(pBLE2902_1);

  //pBLE2902_2 = new BLE2902();
  //pBLE2902_2->setNotifications(true);
  //pCharacteristic_2->addDescriptor(pBLE2902_2);

  // add callback functions here:
  //pCharacteristic->setCallbacks(new CharacteristicCallbacks());
  
  // Start the service
  pCharacteristic->setValue(" ");
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

int BLE_connected()
{
    if(deviceConnected)
    {
        oldDeviceConnected = true;   
    }

    // disconnecting
    if(!deviceConnected && oldDeviceConnected)
    {
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting   
    if(deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }

    return !(deviceConnected ^ 0x01);
}

void Send_BLE_msg()
{
    BLE_packet_t msg_packet = updatePacket();
    StaticJsonDocument<DOC_SIZE_JSON> doc;

    doc["Fuel_Level"] = verify_message_is_null(Fuel_Level_PID, String(msg_packet.Fuel_Level_input));
    doc["Speed"]      = verify_message_is_null(Speed_PID, String(msg_packet.Speed));
    doc["Engine_RPM"] = verify_message_is_null(Engine_RPM_ID, String(msg_packet.Engine_RPM));
    doc["Odometer"]   = verify_message_is_null(Odometer_PID, String(msg_packet.Odometer));

    BLE_sender(doc);
}

void BLE_sender(StaticJsonDocument<DOC_SIZE_JSON>& document)
{
    Serial.print("JSON document Size: "); Serial.println(document.size());
    //memset(msgBLE, 0, sizeof(msgBLE));
    serializeJson(document, msgBLE);
    pCharacteristic->setValue(msgBLE);
    pCharacteristic->notify();
}

void ServerCallbacks::onConnect(BLEServer* pServer)
{
    Serial.println("Client connected");
    deviceConnected = true;
}

void ServerCallbacks::onDisconnect(BLEServer* pServer)
{
    Serial.println("Disconnected");
    deviceConnected = false;
} 
