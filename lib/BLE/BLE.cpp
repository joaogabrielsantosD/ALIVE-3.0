#include "BLE.h"

uint8_t BLEdata[sizeof(BLEmsg_t)];
bool deviceConnected = false;
bool oldDeviceConnected = false;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEDescriptor* pDescr;
BLE2902* pBLE2902;

void setup_BLE()
{
    memset(&BLEdata, 0x00, sizeof(BLEdata));

    // Create the BLE Device
    BLEDevice::init("Dongle OBD");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);    

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,   \
    BLECharacteristic::PROPERTY_NOTIFY |                                    \
    BLECharacteristic::PROPERTY_READ   |                                    \
    BLECharacteristic::PROPERTY_WRITE                                       \
    );                   
    
    // Create a BLE Descriptor
    pDescr = new BLEDescriptor((uint16_t)0x2901);
    pDescr->setValue("Byte array from OBD");
    pCharacteristic->addDescriptor(pDescr);    

    pBLE2902 = new BLE2902();
    pBLE2902->setNotifications(true);
    pCharacteristic->addDescriptor(pBLE2902);
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

void BLE_connected(BLEmsg_t Q)
{   
    if(deviceConnected)
    {
        BLE_Sender(&Q, sizeof(Q) < MAX_BLE_LENGTH ? sizeof(Q) : MAX_BLE_LENGTH);   
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
}

void BLE_Sender(void* T, int len)
{
    memcpy(&BLEdata, (uint8_t *)T, len);
    pCharacteristic->setValue(BLEdata, len);   // seta o valor que a caracteristica notificará (enviar) 
    pCharacteristic->notify();                 // Envia o valor para o smartphone
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
    //retorna ponteiro para o registrador contendo o valor atual da caracteristica
    std::string rxValue = characteristic->getValue(); 
    //verifica se existe dados (tamanho maior que zero)
    if(rxValue.length() > 0) 
    {
        for(int i = 0; i < rxValue.length(); i++) 
        {
            Serial.print(rxValue[i]);
        }
        Serial.println();
    }
}
