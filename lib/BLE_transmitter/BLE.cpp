#include "BLE.h"

/* Defines for debug */
#define PrintJSON
#define BLEdebug

std::string msgBLE = "";
bool deviceConnected = false, oldDeviceConnected = false;
BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pCharacteristic = NULL;

void Init_BLE_Server()
{
    // Create the BLE Device
    BLEDevice::init("ALIVE");

    // Set maximum MTU (512 bytes)
    BLEDevice::setMTU(MAX_BLE_LENGTH);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    pService = pServer->createService(SERVICE_UUID);

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
    // pCharacteristic->setCallbacks(new CharacteristicCallbacks());

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
        oldDeviceConnected = true;

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
        oldDeviceConnected = deviceConnected;

    return deviceConnected;
}

void Send_BLE_msg(BLE_packet_t msg_packet)
{
    StaticJsonDocument<DOC_SIZE_JSON> doc;

    /* Make the JSON packet in the std::string format */
    Make_JSON_packet(doc, msg_packet);
    msgBLE.clear();
    serializeJson(doc, msgBLE);

    /* Print data and length of the std::string */
    #ifdef PrintJSON
        Serial.print("JSON document Size: "); Serial.println(doc.size());
        Serial.println(msgBLE.data());  
        Serial.print("JSON in std::string size: "); Serial.println(msgBLE.length());
        Serial.println();
    #endif

    /* Set and send the value */
    pCharacteristic->setValue(msgBLE);
    pCharacteristic->notify();
    digitalWrite(BLE_DEBUG_LED, digitalRead(BLE_DEBUG_LED) ^ 1);
}

void Make_JSON_packet(StaticJsonDocument<DOC_SIZE_JSON> &JSON, BLE_packet_t &msg_packet)
{
    JSON["Engine_Load"]            = verify_message_is_null(EngineLoad, msg_packet.Calculated_Engine_Load);
    JSON["Engine_Coolant"]         = verify_message_is_null(EngineCollantTemp, msg_packet.Engine_Coolant_Temperature);
    JSON["Fuel_Pressure"]          = verify_message_is_null(FuelPressure, msg_packet.Fuel_Pressure);
    JSON["MAP_SENSOR"]             = verify_message_is_null(IntakeManifoldAbsolutePressure, msg_packet.Intake_Manifold__MAP);
    JSON["Engine_RPM"]             = verify_message_is_null(EngineRPM, msg_packet.Engine_RPM);
    JSON["Speed"]                  = verify_message_is_null(VehicleSpeed, msg_packet.Speed);
    JSON["Throttle_Position"]      = verify_message_is_null(ThrottlePosition, msg_packet.Throttle_Position);
    JSON["Run_Time"]               = verify_message_is_null(RunTimeSinceEngineStart, msg_packet.Run_Time);
    JSON["Distance_traveled_MIL"]  = verify_message_is_null(DistanceTraveledMIL, msg_packet.Distance_traveled_with_MIL_on);
    JSON["Fuel_Level"]             = verify_message_is_null(FuelLevelInput, msg_packet.Fuel_Level_input);
    JSON["Distance_traveled"]      = verify_message_is_null(DistanceTraveledSinceCodeCleared, msg_packet.Distance_traveled);
    JSON["Ambient_Temperature"]    = verify_message_is_null(AmbientAirTemperature, msg_packet.Ambient_Air_Temperature);
    JSON["Engine_Oil_Temperature"] = verify_message_is_null(EngineOilTemperature, msg_packet.Engine_Oil_Temperature);
    JSON["Engine_fuel_rate"]       = verify_message_is_null(EngineFuelRate, msg_packet.Engine_fuel_rate);
    JSON["Odometer"]               = verify_message_is_null(Odometer_PID, msg_packet.Odometer);
    JSON["Acc_X"]               = verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_x);
    JSON["Acc_Y"]                  = verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_y);
    JSON["Acc_Z"]                  = verify_message_is_null(Accelerometer_ST, msg_packet.imu_acc.acc_z);
    //JSON["Ang_X"]            = verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_x);
    //JSON["Ang_Y"]               = verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_y);
    //JSON["Ang_Z"]              = verify_message_is_null(Accelerometer_ST, msg_packet.imu_ang.ang_z);
    JSON["Latitude"] = verify_message_is_null(GPS_ST, msg_packet.gps_data.LAT);
    JSON["Longitude"] = verify_message_is_null(GPS_ST, msg_packet.gps_data.LNG);
    //JSON["Temp_Intern"]              = verify_message_is_null(Accelerometer_ST, msg_packet.acctemp);    
    JSON["DTC"]                    = msg_packet.DTC;
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
