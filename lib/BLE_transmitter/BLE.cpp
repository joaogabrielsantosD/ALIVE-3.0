#include "BLE.h"

/* Defines for debug */
// #define PrintJSON
// #define BLEdebug

BLEHandler BLE;

void BLEHandler::Init_BLE_Server()
{
    // Create the BLE Device
    BLEDevice::init("ALIVE");

    // Set maximum MTU (512 bytes)
    BLEDevice::setMTU(512);

    // Create the BLE Server
    this->pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BLEHandler());

    // Create the BLE Service
    this->pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic( \
        CHARACTERISTIC_UUID,                          \
        BLECharacteristic::PROPERTY_NOTIFY |          \
        BLECharacteristic::PROPERTY_WRITE             \
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
    //pCharacteristic->setCallbacks(new CharacteristicCallbacks());

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

bool BLEHandler::BLE_connected()
{
    if (this->deviceConnected)
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

void BLEHandler::Send_BLE_msg(BLE_packet_t msg_packet)
{
    StaticJsonDocument<DOC_SIZE_JSON> doc;

    doc["x04"] = CircularBufferState.verify_message_is_null(EngineLoad, msg_packet.Calculated_Engine_Load);
    doc["x05"] = CircularBufferState.verify_message_is_null(EngineCollantTemp, msg_packet.Engine_Coolant_Temperature);
    doc["x0A"] = CircularBufferState.verify_message_is_null(FuelPressure, msg_packet.Fuel_Pressure);
    doc["x0B"] = CircularBufferState.verify_message_is_null(IntakeManifoldAbsolutePressure, msg_packet.Intake_Manifold__MAP);
    doc["x0C"] = CircularBufferState.verify_message_is_null(EngineRPM, msg_packet.Engine_RPM);
    doc["x0D"] = CircularBufferState.verify_message_is_null(VehicleSpeed, msg_packet.Speed);
    doc["x11"] = CircularBufferState.verify_message_is_null(ThrottlePosition, msg_packet.Throttle_Position);
    doc["x1F"] = CircularBufferState.verify_message_is_null(RunTimeSinceEngineStart, msg_packet.Run_Time);
    doc["x21"] = CircularBufferState.verify_message_is_null(DistanceTraveledMIL, msg_packet.Distance_traveled_with_MIL_on);
    doc["x2F"] = CircularBufferState.verify_message_is_null(FuelLevelInput, msg_packet.Fuel_Level_input);
    doc["x31"] = CircularBufferState.verify_message_is_null(DistanceTraveledSinceCodeCleared, msg_packet.Distance_traveled);
    doc["x46"] = CircularBufferState.verify_message_is_null(AmbientAirTemperature, msg_packet.Ambient_Air_Temperature);
    doc["x5C"] = CircularBufferState.verify_message_is_null(EngineOilTemperature, msg_packet.Engine_Oil_Temperature);
    doc["x5E"] = CircularBufferState.verify_message_is_null(EngineFuelRate, msg_packet.Engine_fuel_rate);
    doc["xA6"] = CircularBufferState.verify_message_is_null(Odometer_PID, msg_packet.Odometer);
    doc["AcX"] = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_x);
    doc["AcY"] = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_y);
    doc["AcZ"] = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_z);
    //doc["AgX"]  = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_x);
    //doc["AgY"]  = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_y);
    //doc["AgZ"]  = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_z);
    doc["Lat"] = CircularBufferState.verify_message_is_null(GPS_ST, msg_packet.gps_data.LAT);
    doc["Lon"] = CircularBufferState.verify_message_is_null(GPS_ST, msg_packet.gps_data.LNG);
    //doc["Temp_Intern"]              = CircularBufferState.verify_message_is_null(Accelerometer_ST, msg_packet.acctemp);    
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

void BLEHandler::onConnect(BLEServer *pServer)
{
    #ifdef BLEdebug
        Serial.println("Client connected");
    #endif
    this->deviceConnected = true;
}

void BLEHandler::onDisconnect(BLEServer *pServer)
{
    #ifdef BLEdebug
        Serial.println("Disconnected");
    #endif
    this->deviceConnected = false;
}

// void CharacteristicCallbacks::onWrite(BLECharacteristic *SenderCharacteristic)
// {
//     std::string value = SenderCharacteristic->getValue();

//     if (SenderCharacteristic->getLength() > 0)
//     {
//         for (int i = 0; i < SenderCharacteristic->getLength(); i++)
//             value[i] = toupper(value[i]);

//         if (value.compare("DTC") == 0)
//         {
//             #ifdef BLEdebug
//                 Serial.println("DTC requisitado");
//             #endif
//             Call_DTC_mode3();
//         }

//         else if (value.compare("APAGAR DTC") == 0)
//         {
//             #ifdef BLEdebug
//                 Serial.println("Codigo DTC apagado");
//             #endif
//             //cleanDTC();
//         }
//     }
// }
