#include "AcquisitionData.h"

CAN_Messages CAN_msg;
BLE_packet_t volatile_packet;
static const uint32_t GPS_baudrate = 4800;

/* Module variables */
pthread_mutex_t acq_mutex = PTHREAD_MUTEX_INITIALIZER;
TinyGPSPlus gps_const;
MPU9250 mpu_const;

/* Debug Variables */
//#define debug_when_receive_byte;
//#define debug_modules

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

  #ifdef debug_modules
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

  #ifdef debug_modules
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

    #ifdef debug_when_receive_byte
      for (int i = 0; i < length; i++)
      {
        Serial.print(info_can[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    #endif

    CAN_msg.Handling_Message((uint8_t*)info_can, &volatile_packet);
  }
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
