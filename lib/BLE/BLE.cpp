#include "BLE.h"

bool deviceConnected = false, oldDeviceConnected = false;
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
  pCharacteristic->setCallbacks(new CharacteristicCallbacks());
  
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

void BLE_Sender(void* T, uint32_t len)
{
    Serial.println("Enviando mensagme por BLE");
    uint8_t msg[len];
    memcpy(&msg, (uint8_t*)T, len);
    //for(int i = 0; i < len; i++) Serial.println(msg[i]);
    pCharacteristic->setValue(msg, len); 
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

void CharacteristicCallbacks::onWrite(BLECharacteristic* characteristic)
{
    std::string pChar2_value_stdstr = characteristic->getValue();
    String pChar2_value_string = String(pChar2_value_stdstr.c_str());
    Serial.println(pChar2_value_string);
}
