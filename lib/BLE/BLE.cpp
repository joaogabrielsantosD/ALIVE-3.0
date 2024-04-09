#include "BLE.h"

bool deviceConnected = false;
BLECharacteristic *characteristicTX; //através desse objeto iremos enviar dados para o client

#define SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e”
#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91”

void setup_BLE()
{ // Create the BLE Device
    BLEDevice::init("Dongle OBDII"); // nome do dispositivo bluetooth

    // Create the BLE Server
    BLEServer *server = BLEDevice::createServer(); //cria um BLE server 

    server->setCallbacks(new ServerCallbacks()); //seta o callback do server

    // Create the BLE Service
    BLEService *service = server->createService(SERVICE_UUID);

    // Create a BLE Characteristic para envio de dados
    BLECharacteristic *characteristic = service->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE
                     );

    //characteristicTX->addDescriptor(new BLE2902());


/* -------------------------------------------------------------------------------------------------------------*/

// Create a BLE Characteristic para recebimento de dados
    BLECharacteristic *characteristic = service->createCharacteristic(
                                                      CHARACTERISTIC_UUID_RX,
                                                      BLECharacteristic::PROPERTY_WRITE
                                                    );

    characteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    service->start();

    // Start advertising (descoberta do ESP32)
    server->getAdvertising()->start();


}


void BLE_Sender()
{      

                
                characteristicTX->setValue("ENVIA CHAR"); //seta o valor que a caracteristica notificará (enviar) 
                characteristicTX->notify(); // Envia o valor para o smartphone
}
                

//callback para eventos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic) {
          //retorna ponteiro para o registrador contendo o valor atual da caracteristica
          std::string rxValue = characteristic->getValue(); 
          //verifica se existe dados (tamanho maior que zero)
          if (rxValue.length() > 0) 
          {
             for (int i = 0; i < rxValue.length(); i++) {
             Serial.print(rxValue[i]);
               }
               Serial.println();
          }
     } 
};

//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};