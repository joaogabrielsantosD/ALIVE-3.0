#include "AcquisitionData.h"

BLE_packet_t volatile_packet;
static const uint32_t GPS_baudrate = 4800;
/* Module variables */
pthread_mutex_t acq_mutex = PTHREAD_MUTEX_INITIALIZER;
TinyGPSPlus gps_const;
MPU9250 mpu_const;
/* Debug Variables */
bool debug_when_receive_byte = false;
bool debug_when_receive_info = false;
/*
 * 0 - Não Printa nada na Serial
 * 1 - Print apenas mensagens CAN
 * 2 - Print apenas DTC
 * 3 - Print apenas dados dos modulos
 * 4 - Print todos
 */
#define debug_message 0

void start_module_device()
{
  memset(&volatile_packet, 0, sizeof(BLE_packet_t));
  volatile_packet.DTC = "null";

  // init the gps serial command AT communication
  SerialAT.begin(GPS_baudrate);

  // init the MPU
  Wire.begin();

  if (mpu_const.setup(0x68))
  {
    mpu_const.verbose(true);
    mpu_const.calibrateAccelGyro();
    mpu_const.verbose(false);
    save_flag_imu_parameter(true);
  }
}

void acq_function(int acq_mode)
{
  pthread_t th;

  pthread_mutex_init(&acq_mutex, NULL);

  switch (acq_mode)
  {
    case Accelerometer_ST:
      pthread_create(&th, NULL, &imu_acq_function, NULL);
      pthread_join(th, NULL);
      break;

    case GPS_ST:
      pthread_create(&th, NULL, &gps_acq_function, NULL);
      pthread_join(th, NULL);
      break;
    
    default:
      Handling_CAN_msg();
      break;
  }
  
  pthread_mutex_destroy(&acq_mutex);
}

/*================================ Accelerometer && GPS functions ================================*/
ThreadHandle_t imu_acq_function(void *arg)
{
  if (mpu_const.update())
  {
    pthread_mutex_lock(&acq_mutex);
    volatile_packet.imu_acc.acc_x = mpu_const.getRoll();
    volatile_packet.imu_acc.acc_y = mpu_const.getPitch();
    volatile_packet.imu_acc.acc_z = mpu_const.getYaw();
    pthread_mutex_unlock(&acq_mutex);
  }

  #if debug_message == 3 || debug_message == 4
    Serial.printf("Acc x: %.2f\r\n", volatile_packet.imu_acc.acc_x);
    Serial.printf("Acc y: %.2f\r\n", volatile_packet.imu_acc.acc_y);
    Serial.printf("Acc z: %.2f\r\n", volatile_packet.imu_acc.acc_z);
  #endif

  return NULL;
}

ThreadHandle_t gps_acq_function(void *arg)
{
  while (SerialAT.available() > 0)
  {
    if (gps_const.encode(SerialAT.read()))
    {
      if (gps_const.location.isValid())
      {
        pthread_mutex_lock(&acq_mutex);
        volatile_packet.gps_data.LAT = gps_const.location.lat();
        volatile_packet.gps_data.LNG = gps_const.location.lng();
        pthread_mutex_unlock(&acq_mutex);
      }
    }
  }

  #if debug_message == 3 || debug_message == 4
    Serial.printf("LAT: %lf\r\n", volatile_packet.gps_data.LAT);
    Serial.printf("LNG: %lf\r\n", volatile_packet.gps_data.LAT);
  #endif

  return NULL;
}

/*================================== CAN Acquisition functions ==================================*/
void Handling_CAN_msg()
{
  uint8_t length = 8;
  uint32_t ID = 0;
  unsigned char info_can[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  while (msg_receive())
  {
    get_msg(info_can, ID, length);

    if (debug_when_receive_byte)
    {
      for (int i = 0; i < length; i++)
      {
        Serial.print(info_can[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    }

    switch (info_can[2])
    {
      case 0x41:
      { // case for PID support
        if (info_can[0] == 0x10)
        {
          if (info_can[3] == PIDs1)
            Storage_PIDenable_bit(info_can, PID_to_index_1);
          else if (info_can[3] == PIDs2)
            Storage_PIDenable_bit(info_can, PID_to_index_2);
          else if (info_can[3] == PIDs3)
            Storage_PIDenable_bit(info_can, PID_to_index_3);
          else if (info_can[3] == PIDs4)
            Storage_PIDenable_bit(info_can, PID_to_index_4);
          else if (info_can[3] == PIDs5)
            Storage_PIDenable_bit(info_can, PID_to_index_5);
        }
  
        break;
      }
  
      case EngineLoad:
      {
        float A = info_can[3];
        volatile_packet.Calculated_Engine_Load = (100 * A) / 255;
        
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Calculated engine load value:  %f\r\n", volatile_packet.Calculated_Engine_Load);
        #endif
  
        break;
      }
  
      case EngineCollantTemp:
      {
        float A = info_can[3];
        volatile_packet.Engine_Coolant_Temperature = A - 40;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Engine Coolant Temperature:  %f\r\n", volatile_packet.Engine_Coolant_Temperature);
        #endif
  
        break;
      }
  
      case ShortTermFuel_Bank1:
      {
        float A = info_can[3];
        volatile_packet.Short_Long_Term.ShortTermFuel.Bank1 = (A - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("ShortTermFuel_Bank1:  %f\r\n", volatile_packet.Short_Long_Term.ShortTermFuel.Bank1);
        #endif

        break;
      }

      case LongTermFuel_Bank1:
      {
        float A = info_can[3];
        volatile_packet.Short_Long_Term.LongTermFuel.Bank1 = (A - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("LongTermFuel_Bank1:  %f\r\n", volatile_packet.Short_Long_Term.LongTermFuel.Bank1);
        #endif

        break;
      }

      case ShortTermFuel_Bank2:
      {
        float A = info_can[3];
        volatile_packet.Short_Long_Term.ShortTermFuel.Bank2 = (A - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("ShortTermFuel_Bank2:  %f\r\n", volatile_packet.Short_Long_Term.ShortTermFuel.Bank2);
        #endif

        break;
      }

      case LongTermFuel_Bank2:
      {
        float A = info_can[3];
        volatile_packet.Short_Long_Term.LongTermFuel.Bank2 = (A - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("LongTermFuel_Bank2:  %f\r\n", volatile_packet.Short_Long_Term.LongTermFuel.Bank2);
        #endif

        break;
      }

      case FuelPressure:
      {
        float A = info_can[3];
        volatile_packet.Fuel_Pressure = 3 * A;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Fuel Pressure:  %f\r\n", volatile_packet.Fuel_Pressure);
        #endif
  
        break;
      }
  
      case IntakeManifoldAbsolutePressure:
      {
        float A = info_can[3];
        volatile_packet.Intake_Manifold__MAP = A;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Intake manifold absolute pressure(MAP):  %f\r\n", volatile_packet.Intake_Manifold__MAP);
        #endif
  
        break;
      }
  
      case EngineRPM:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Engine_RPM = (256 * A + B) / 4;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Engine RPM:  %f\r\n", volatile_packet.Engine_RPM);
        #endif
  
        break;
      }
  
      case VehicleSpeed:
      {
        float A = info_can[3];
        volatile_packet.Speed = A;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Vehicle speed:  %f\r\n", volatile_packet.Speed);
        #endif
  
        break;
      }
  
      case TimingAdvance:
      {
        float A = info_can[3];
        volatile_packet.Timing_Advance = (A / 2) - 64;

        #if debug_message == 1 || debug_message == 4
          Serial.printf("TimingAdvance:  %f\r\n", volatile_packet.Timing_Advance);
        #endif

        break;
      }

      case IntakeAirTemperature:
      {
        float A = info_can[3];
        volatile_packet.Intake_Air_Temperature = A - 40;

        #if debug_message == 1 || debug_message == 4
          Serial.printf("IntakeAirTemperature:  %f\r\n", volatile_packet.Intake_Air_Temperature);
        #endif
        
        break;
      }

      case MAFairFlowRate:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.MAF_Air_FlowRate = ((A * 256) + B) / 100;

        #if debug_message == 1 || debug_message == 4
          Serial.printf("MAFairFlowRate:  %f\r\n", volatile_packet.MAF_Air_FlowRate);
        #endif

        break;
      }      

      case ThrottlePosition:
      {
        float A = info_can[3];
        volatile_packet.Throttle_Position = (100 * A) / 255;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Throttle position:  %f\r\n", volatile_packet.Throttle_Position);
        #endif
  
        break;
      }
  
      case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor1 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss1 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor1);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss1);
          }
        #endif
        
        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor2 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss2 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor2);
          
          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss2);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor3 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss3 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor3);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss3);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor4 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss4 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank1_Sensor4);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk1_ss4);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor1 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss1 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor1);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss1);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor2 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss2 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor2);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss2);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor3 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss3 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor3);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss3);
          }
        #endif

        break;
      }

      case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4:
      {
        float A = info_can[3], B = info_can[4]; 
        volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor4 = A / 200;

        if (B != 0xFF)
          volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss4 = (B - 128) * (100 / 128);

        #if debug_message == 1 || debug_message == 4
          Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4:  %f\r\n", 
          volatile_packet.Short_Long_Term.OxygenSensorVolt._Bank2_Sensor4);

          if (B != 0xFF)
          {
            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4 ( 2 ):  %f\r\n", 
            volatile_packet.Short_Long_Term.OxygenSensorVolt.bk2_ss4);
          }
        #endif

        break;
      }

      case RunTimeSinceEngineStart:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Run_Time = 256 * A + B;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Run Time since engine start:  %f\r\n", volatile_packet.Run_Time);
        #endif
  
        break;
      }
  
      case DistanceTraveledMIL:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Distance_traveled_with_MIL_on = 256 * A + B;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Distance traveled with malfunction indicator lamp (MIL) on :  %f\r\n", volatile_packet.Distance_traveled_with_MIL_on);
        #endif
  
        break;
      }
  
      case FuelLevelInput:
      {
        float A = info_can[3];
        volatile_packet.Fuel_Level_input = (100 * A) / 255;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Fuel Level Input:  %f\r\n", volatile_packet.Fuel_Level_input);
        #endif
  
        break;
      }
  
      case DistanceTraveledSinceCodeCleared:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Distance_traveled = 256 * A + B;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Distance traveled since codes cleared:  %f\r\n", volatile_packet.Distance_traveled);
        #endif
  
        break;
      }
  
      case AmbientAirTemperature:
      {
        float A = info_can[3];
        volatile_packet.Ambient_Air_Temperature = A - 40;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Ambient air temperature:  %f\r\n", volatile_packet.Ambient_Air_Temperature);
        #endif
  
        break;
      }
  
      case EngineOilTemperature:
      {
        float A = info_can[3];
        volatile_packet.Engine_Oil_Temperature = A - 40;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Engine Oil Temperature:  %f\r\n", volatile_packet.Engine_Oil_Temperature);
        #endif
  
        break;
      }
  
      case EngineFuelRate:
      {
        float A = info_can[3], B = info_can[4];
        volatile_packet.Engine_fuel_rate = (256 * A + B) / 20;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Engine Fuel rate:  %f\r\n", volatile_packet.Engine_fuel_rate);
        #endif
  
        break;
      }
  
      case Odometer_PID:
      {
        float A = info_can[3], B = info_can[4], C = info_can[5], D = info_can[6];
        volatile_packet.Odometer = ((A * pow(2, 24)) + (B * pow(2, 16)) + (C * pow(2, 8)) + D) / 10;
  
        #if debug_message == 1 || debug_message == 4
          Serial.printf("Odometer:  %f\r\n", volatile_packet.Odometer);
        #endif
  
        break;
      }
  
      default:
      {
        if (info_can[1] == 0x43)
        {
          volatile_packet.DTC = make_DTC_code(info_can[3], info_can[4]);
  
          #if debug_message == 2 || debug_message == 4
            Serial.print("Codigo de leitura de falhas: ");
            Serial.println(volatile_packet.DTC);
          #endif
        }
  
        break;
      }
    }
  }
}

String make_DTC_code(uint8_t first_msg, uint8_t second_msg)
{
  String code_dtc = "";
  char *characters = (char *)malloc(sizeof(char) * 3);

  // First character
  char _first_char = (first_msg == 0 ? 'P' : first_msg == 1 ? 'C'         \
                                           : first_msg == 2 ? 'B'         \
                                           : first_msg == 3 ? 'U' : 'Z'); \

  if (_first_char == 'Z')
    return {"null"};

  code_dtc += String(_first_char); // write the first char

  uint8_t _second_char = (second_msg & 0x03);
  _second_char = (_second_char != 0 && _second_char != 3 ? _second_char ^= 3 : _second_char);
  code_dtc += String(_second_char); // write the second char
  
  uint8_t _third_fourth_fifth_char[] =
  {
    (uint8_t)((second_msg & 0xF0) >> 4), (uint8_t)((second_msg & 0xF0) >> 4), (uint8_t)(second_msg & 0x0F)
  };

  for (int8_t i = 0; i < sizeof(_third_fourth_fifth_char); i++)
  { // write the third, fourth and fifth char (in this order each interation)
    *(characters + i) = (char)verify_char(_third_fourth_fifth_char[i]);

    if (*(characters + i) != 'Z')
      code_dtc += String(*(characters + i));
    else
      code_dtc += String(_third_fourth_fifth_char[i]);
  }

  free(characters);

  return code_dtc;
}

void cleanDTC()
{
  volatile_packet.DTC.clear();
  volatile_packet.DTC = "null";
}

/*================================== Packet Message Functions ==================================*/
BLE_packet_t updatePacket()
{
  return volatile_packet;
}
