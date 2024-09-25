#include "BLE.h"

/* Defines for debug */
#define PrintJSON
#define BLEdebug

bool deviceConnected = false, oldDeviceConnected = false;
std::string msgBLE = "";
BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pCharacteristic = NULL;

void Init_BLE_Server()
{
    // Create the BLE Device
    BLEDevice::init("Dongle OBD");

    // Set maximum MTU (512 bytes)
    BLEDevice::setMTU(512);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic( \
        CHARACTERISTIC_UUID,                          \
        BLECharacteristic::PROPERTY_NOTIFY |          \
        BLECharacteristic::PROPERTY_WRITE  |          \
        BLECharacteristic::PROPERTY_READ              \
        );

    // Create a BLE Descriptor
    // pDescr_1 = new BLEDescriptor((uint16_t)0x2901);
    // pDescr_1->setValue("A very interesting variable");
    // pCharacteristic_1->addDescriptor(pDescr_1);

    // Add the BLE2902 Descriptor because we are using "PROPERTY_NOTIFY"
    // pBLE2902_1 = new BLE2902();
    // pBLE2902_1->setNotifications(true);
    // pCharacteristic_1->addDescriptor(pBLE2902_1);

    // pBLE2902_2 = new BLE2902();
    // pBLE2902_2->setNotifications(true);
    // pCharacteristic_2->addDescriptor(pBLE2902_2);

    // add callback functions here:
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    pCharacteristic->setValue(" ");
    pService->start();

    // Start advertising
    pinMode(BLE_DEBUG_LED, OUTPUT);
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    #ifdef BLEdebug
     Serial.println("Waiting a client connection to notify...");
    #endif
}

bool BLE_connected()
{
    if (deviceConnected)
    {
        oldDeviceConnected = true;
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        digitalWrite(BLE_DEBUG_LED, LOW);
        pServer->startAdvertising(); // restart advertising
        #ifdef BLEdebug
            Serial.println("start advertising");
        #endif
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }

    return deviceConnected;
}

void Send_BLE_msg()
{
    BLE_packet_t msg_packet = updatePacket();
    StaticJsonDocument<DOC_SIZE_JSON> doc;

    doc["Engine_Load"]            = verify_message_is_null(EngineLoad, msg_packet.Calculated_Engine_Load);
    doc["Engine_Coolant"]         = verify_message_is_null(EngineCollantTemp, msg_packet.Engine_Coolant_Temperature);
    doc["Fuel_Pressure"]          = verify_message_is_null(FuelPressure, msg_packet.Fuel_Pressure);
    doc["MAP_SENSOR"]             = verify_message_is_null(IntakeManifoldAbsolutePressure, msg_packet.Intake_Manifold__MAP);
    doc["Engine_RPM"]             = verify_message_is_null(EngineRPM, msg_packet.Engine_RPM);
    doc["Speed"]                  = verify_message_is_null(VehicleSpeed, msg_packet.Speed);
    doc["Throttle_Position"]      = verify_message_is_null(ThrottlePosition, msg_packet.Throttle_Position);
    doc["Run_Time"]               = verify_message_is_null(RunTimeSinceEngineStart, msg_packet.Run_Time);
    doc["Distance_traveled_MIL"]  = verify_message_is_null(DistanceTraveledMIL, msg_packet.Distance_traveled);
    doc["Fuel_Level"]             = verify_message_is_null(FuelLevelInput, msg_packet.Fuel_Level_input);
    doc["Distance_traveled"]      = verify_message_is_null(DistanceTraveledSinceCodeCleared, msg_packet.Distance_traveled_with_MIL_on);
    doc["Ambient_Temperature"]    = verify_message_is_null(AmbientAirTemperature, msg_packet.Ambient_Air_Temperature);
    doc["Engine_Oil_Temperature"] = verify_message_is_null(EngineOilTemperature, msg_packet.Engine_Oil_Temperature);
    doc["Engine_fuel_rate"]       = verify_message_is_null(EngineFuelRate, msg_packet.Engine_fuel_rate);
    doc["Odometer"]               = verify_message_is_null(Odometer_PID, msg_packet.Odometer);
    doc["Ang_X"]                  = verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_x);
    doc["Ang_Y"]                  = verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_y);
    doc["Temp_Intern"]            = verify_message_is_null(Accelerometer_ST, msg_packet.acctemp);
    doc["Latitude"]               = verify_message_is_null(GPS_ST, msg_packet.gps_data.LAT);
    doc["Longitude"]              = verify_message_is_null(GPS_ST, msg_packet.gps_data.LNG);    
    doc["DTC"]                    = msg_packet.DTC;

    /* Make the JSON packet in the std::string format */
    msgBLE.clear();
    serializeJson(doc, msgBLE);

    /* Print data and length of the std::string */
    #ifdef PrintJSON
        Serial.print("JSON document Size: "); Serial.println(doc.size());
        Serial.println(msgBLE.data());  
        Serial.print("JSON in std::string size: "); Serial.println(msgBLE.length());
    #endif

    /* Set and send the value */
    pCharacteristic->setValue(msgBLE);
    pCharacteristic->notify();
    digitalWrite(BLE_DEBUG_LED, digitalRead(BLE_DEBUG_LED) ^ 1);
}

void ServerCallbacks::onConnect(BLEServer *pServer)
{
    #ifdef BLEdebug
        Serial.println("Client connected");
    #endif
    deviceConnected = true;
}

void ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    #ifdef BLEdebug
        Serial.println("Disconnected");
    #endif
    deviceConnected = false;
}

void CharacteristicCallbacks::onWrite(BLECharacteristic *SenderCharacteristic)
{
    std::string value = SenderCharacteristic->getValue();

    if (SenderCharacteristic->getLength() > 0)
    {
        for (int i = 0; i < SenderCharacteristic->getLength(); i++)
            value[i] = toupper(value[i]);

        if (value.compare("DTC") == 0)
        {
            #ifdef BLEdebug
                Serial.println("DTC requisitado");
            #endif
            Call_DTC_mode3();
        }

        else if (value.compare("APAGAR DTC") == 0)
        {
            #ifdef BLEdebug
                Serial.println("Codigo DTC apagado");
            #endif
            cleanDTC();
        }
    }
}
