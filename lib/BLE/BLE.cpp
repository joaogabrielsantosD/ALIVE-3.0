#include "BLE.h"

uint8_t BLEdata[sizeof(BLEmsg_t)];
bool deviceConnected = false;
BLECharacteristic* characteristicTX; //através desse objeto iremos enviar dados para o client

void setup_BLE()
{
    memset(&BLEdata, 0x00, sizeof(BLEdata));

    // Create the BLE Device
    BLEDevice::init("Dongle OBDII"); // nome do dispositivo bluetooth

    // Create the BLE Server
    BLEServer* server = BLEDevice::createServer(); //cria um BLE server 

    server->setCallbacks(new ServerCallbacks()); //seta o callback do server

    // Create the BLE Service
    BLEService* service = server->createService(SERVICE_UUID);

    // Create a BLE Characteristic para envio de dados
    BLECharacteristic* characteristic = service->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

    //characteristicTX->addDescriptor(new BLE2902());

 /* -------------------------------------------------------------------------------------------------------------------------------*/

    // Create a BLE Characteristic para recebimento de dados
    //BLECharacteristic *CHARACTERISTIC = service->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

    //CHARACTERISTIC->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    service->start();

    // Start advertising (descoberta do ESP32)
    server->getAdvertising()->start();
}

void BLE_Sender(void* T, int len)
{
    memcpy(&BLEdata, (uint8_t *)T, len);
    characteristicTX->setValue(BLEdata, len);   // seta o valor que a caracteristica notificará (enviar) 
    characteristicTX->notify();                 // Envia o valor para o smartphone
}

void ServerCallbacks::onConnect(BLEServer* pServer)
{
    deviceConnected = true;
}

void ServerCallbacks::onDisconnect(BLEServer* pServer)
{
    deviceConnected = false;
} 

void CharacteristicCallbacks::onWrite(BLECharacteristic* characteristic)
{
    //retorna ponteiro para o registrador contendo o valor atual da caracteristica
    std::string rxValue = characteristic->getValue(); 
    //verifica se existe dados (tamanho maior que zero)
    if(rxValue.length() > 0) 
    {
        for (int i = 0; i < rxValue.length(); i++) 
        {
            Serial.print(rxValue[i]);
        }
        Serial.println();
    }
}
