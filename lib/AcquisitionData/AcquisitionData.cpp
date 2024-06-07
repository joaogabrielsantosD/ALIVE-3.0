#include "AcquisitionData.h"

BLE_packet_t volatile_packet = defaultValue();
static const uint32_t GPS_baudrate = 4800;
/* Module variables */
pthread_mutex_t acq_mutex;
TinyGPSPlus gps_const;
MPU9250 mpu_const;
/* Debug Variables */
boolean imu_init = false;
bool debug_when_receive = true; // variable to enable the Serial when receive

void start_module_device()
{
  // init the gps serial command AT communication
  SerialAT.begin(GPS_baudrate);

  // init the MPU
  Wire.begin();

  if(mpu_const.setup(0x68))
  {
    mpu_const.verbose(true);
    mpu_const.calibrateAccelGyro();
    mpu_const.verbose(false);
    imu_init = true;
    save_flag_imu_parameter(imu_init);
  }
}

void acq_function(int acq_mode)
{
  pthread_t th;

  pthread_mutex_init(&acq_mutex, NULL);

  switch(acq_mode)
  {
    case Accelerometer_ST:
      pthread_create(&th, NULL, &imu_acq_function, NULL);
      pthread_join(th, NULL);
      break;

    case GPS_ST:
      pthread_create(&th, NULL, &gps_acq_function, NULL);
      pthread_join(th, NULL);
      break;
  }

  pthread_mutex_destroy(&acq_mutex);
}

/*================================ Accelerometer && GPS functions ================================*/
ThreadHandle_t imu_acq_function(void *arg)
{
  if(mpu_const.update() && imu_init)
  {
    pthread_mutex_lock(&acq_mutex);
    volatile_packet.imu_acc.acc_x = mpu_const.getRoll();
    volatile_packet.imu_acc.acc_y = mpu_const.getPitch();
    volatile_packet.imu_acc.acc_z = mpu_const.getYaw();
    pthread_mutex_unlock(&acq_mutex); 
  }

  return NULL;
}

ThreadHandle_t gps_acq_function(void *arg)
{
  while(SerialAT.available() > 0)
  {
    if(gps_const.encode(SerialAT.read()))
    {
      if(gps_const.location.isValid())
      {
        pthread_mutex_lock(&acq_mutex);
        volatile_packet.gps_data.LAT = gps_const.location.lat();
        volatile_packet.gps_data.LNG = gps_const.location.lng();
        pthread_mutex_unlock(&acq_mutex);
      }
    }
  }

  return NULL;
}

/*================================== CAN Acquisition functions ==================================*/
void MsgRec_Treatment()
{
  uint8_t length = 8;
  uint32_t ID = 0;
  unsigned char info_can[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  while(msg_receive())
  {
    get_msg(info_can, ID, length);

    if(false)
    {
      for(int i = 0; i < length; i++)
      {
        Serial.print(info_can[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    }

    switch(info_can[2])
    {
      case 0x41:
      { // case for PID support
        if(info_can[0]==0x10)
        {
          if(info_can[3]==PIDs1) Storage_PIDenable_bit(info_can, PID_to_index_1);
          else if(info_can[3]==PIDs2) Storage_PIDenable_bit(info_can, PID_to_index_2);
          else if(info_can[3]==PIDs3) Storage_PIDenable_bit(info_can, PID_to_index_3);
          else if(info_can[3]==PIDs4) Storage_PIDenable_bit(info_can, PID_to_index_4);
          else if(info_can[3]==PIDs5) Storage_PIDenable_bit(info_can, PID_to_index_5);
        }

        break;
      }

      case Engine_LoadP_ID:
      {
        float A = info_can[4];
        volatile_packet.Calculated_Engine_Load = (100*A)/255;
        if(debug_when_receive) Serial.printf("Calculated engine load value:  %f\r\n", volatile_packet.Calculated_Engine_Load);
      
        break;
      }

      case Engine_CoolantP_ID:
      {
        float A = info_can[4];
        volatile_packet.Engine_Coolant_Temperature = A - 40;
        if(debug_when_receive) Serial.printf("Engine Coolant Temperature:  %f\r\n", volatile_packet.Engine_Coolant_Temperature);
      
        break;
      }

      case Fuel_Pressure_PID:
      {
        float A = info_can[4];
        volatile_packet.Fuel_Pressure = 3*A;
        if(debug_when_receive) Serial.printf("Fuel Pressure:  %f\r\n", volatile_packet.Fuel_Pressure);
      
        break;
      }

      case MAP_sensor_PID:
      {
        float A = info_can[4];
        volatile_packet.Intake_Manifold_Absolute_Pressure_MAP = A;
        if(debug_when_receive) Serial.printf("Intake manifold absolute pressure(MAP):  %f\r\n", volatile_packet.Intake_Manifold_Absolute_Pressure_MAP);
      
        break;
      }

      case Engine_RPM_ID:
      {
        float A = info_can[4], B = info_can[5];
        volatile_packet.Engine_RPM = (256*A + B)/4;
        if(debug_when_receive) Serial.printf("Engine RPM:  %f\r\n", volatile_packet.Engine_RPM);
      
        break;
      }

      case Speed_PID:
      {
        float A = info_can[4];
        volatile_packet.Speed = A;
        if(debug_when_receive) Serial.printf("Vehicle speed:  %f\r\n", volatile_packet.Speed);
      
        break;
      }

      case Throttle_Pos_PID:
      {
        float A = info_can[4];
        volatile_packet.Throttle_Position = (100*A)/255;
        if(debug_when_receive) Serial.printf("Throttle position:  %f\r\n", volatile_packet.Throttle_Position);
      
        break;
      }

      case Run_Time_PID:
      {
        float A = info_can[4], B = info_can[5];
        volatile_packet.Run_Time = 256*A + B;
        if(debug_when_receive) Serial.printf("Run Time since engine start:  %f\r\n", volatile_packet.Run_Time);
      
        break;
      }

      case Distance_on_MIL_PID:
      {
        float A = info_can[4], B = info_can[5];
        volatile_packet.Distance_traveled_with_MIL_on = 256*A + B;
        if(debug_when_receive) Serial.printf("Distance traveled with malfunction indicator lamp (MIL) on :  %f\r\n", volatile_packet.Distance_traveled_with_MIL_on);
      
        break;
      }

      case Fuel_Level_PID:
      {
        float A = info_can[4];
        volatile_packet.Fuel_Level_input = (100*A)/255;
        if(debug_when_receive) Serial.printf("Fuel Level Input:  %f\r\n", volatile_packet.Fuel_Level_input);
      
        break;
      }

      case Distance_Travel_PID:
      {
        float A = info_can[4], B = info_can[5];
        volatile_packet.Distance_traveled_since_codes_cleared = 256*A + B;
        if(debug_when_receive) Serial.printf("Distance traveled since codes cleared:  %f\r\n", volatile_packet.Distance_traveled_since_codes_cleared);
      
        break;
      }

      case Ambient_Temp_PID:
      {
        float A = info_can[4];
        volatile_packet.Ambient_Air_Temperature = A - 40;
        if(debug_when_receive) Serial.printf("Ambient air temperature:  %f\r\n", volatile_packet.Ambient_Air_Temperature);
      
        break;
      }

      case Engine_Oil_PID:
      {
        float A = info_can[4];
        volatile_packet.Engine_Oil_Temperature = A - 40;
        if(debug_when_receive) Serial.printf("Engine Oil Temperature:  %f\r\n", volatile_packet.Engine_Oil_Temperature);
      
        break;
      }

      case Engine_FuelRate_PID:
      {
        float A = info_can[4], B = info_can[5];
        volatile_packet.Engine_fuel_rate = (256*A + B)/20;
        if(debug_when_receive) Serial.printf("Engine Fuel rate:  %f\r\n", volatile_packet.Engine_fuel_rate);
      
        break;
      }

      case Odometer_PID:
      {
        float A = info_can[4], B = info_can[5], C = info_can[6], D = info_can[7];
        volatile_packet.Odometer = ((A*pow(2,24)) + (B*pow(2,16)) + (C*pow(2,8)) + D)/10;
        if(debug_when_receive) Serial.printf("Odometer:  %f\r\n", volatile_packet.Odometer);
      
        break;
      }
    }
  }
}

/*================================== Packet Message Functions ==================================*/
BLE_packet_t defaultValue()
{
  BLE_packet_t t;
  memset(&t, 0x00, sizeof(BLE_packet_t));
  return t;
}

BLE_packet_t updatePacket()
{
  return volatile_packet;
}
