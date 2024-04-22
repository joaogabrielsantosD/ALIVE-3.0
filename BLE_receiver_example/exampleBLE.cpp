
#include <Arduino.h>
#include "BLEDevice.h"

/* Specify the Service UUID of Server */
static BLEUUID serviceUUID("acc1f4ef-4fcf-4f90-882e-0a666da9f321");
/* Specify the Characteristic UUID of Server */
static BLEUUID    charUUID("50e6cbc6-5aff-4423-974c-0e27959453c3");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

typedef struct 
{
    float EngineCollant, Engine_rpm, vehicle_speed, fuellevel, Distance_travel;
} BLEmsg_t;

BLEmsg_t mmm;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  //Serial.println(pData[2]);
  //Serial.println(pBLERemoteCharacteristic->readValue().c_str());
  memcpy(&mmm, (BLEmsg_t*)pData, length);
  Serial.println(mmm.Distance_travel);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient* pclient)
  {
    Serial.println("Connected");
  }

  void onDisconnect(BLEClient* pclient)
  {
    connected = false;
    Serial.println("onDisconnect");
  }
};

/* Scan for BLE servers and find the first one that advertises the service we are looking for. */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
 /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }
  }
};

bool connectToServer();

void setup() 
{
  Serial.begin(9600);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("ESP32-BLE-Client");

  /* Retrieve a Scanner and set the callback we want to use to be informed when we
     have detected a new device.  Specify that we want active scanning and start the
     scan to run for 5 seconds. */
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void loop() 
{
  /* If the flag "doConnect" is true, then we have scanned for and found the desired
     BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
     connected we set the connected flag to be true. */
  if (doConnect == true)
  {
    if (connectToServer())
    {
      Serial.println("We are now connected to the BLE Server.");
    } 
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  /* If we are connected to a peer BLE Server, update the characteristic each time we are reached
     with the current time since boot */
  if (connected)
  {
    String newValue = "Time since boot: " + String(millis()/2000);
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    /* Set the characteristic's value to be the array of bytes that is actually a string */
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    /* You can see this value updated in the Server's Characteristic */
  }
  else if(doScan)
  {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  uint32_t t = millis();
  while(!connected)
  {
    if((millis() - t) > 5000) esp_restart();
  }
  

  delay(100); /* Delay a second between loops */
}

/* Start connection to the BLE Server */
bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
    
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

    /* Connect to the remote BLE Server */
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

    /* Obtain a reference to the service we are after in the remote BLE server */
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");


  /* Obtain a reference to the characteristic in the service of the remote BLE server */
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  /* Read the value of the characteristic */
  /* Initial value is 'Hello, World!' */
  if(pRemoteCharacteristic->canRead())
  {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if(pRemoteCharacteristic->canNotify())
  {
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  }

    connected = true;
    return true;
}
