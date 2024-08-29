#include "AcquisitionData.h"

CAN_Messages CAN_msg;
BLE_packet_t volatile_packet;
static const uint32_t GPS_baudrate = 4800;

/* Module variables */
TinyGPSPlus gps_const;
#define MPU9250_ADDR 0x68
MPU9250_WE MPU9250 = MPU9250_WE(MPU9250_ADDR);

/* Debug Variables */

//#define debug_when_receive_byte  
//#define debug_acc             //Print on Serial  paramenters
//#define debug_GPS             //Print on Serial the paramenters


void start_module_device()
{
  memset(&volatile_packet, 0, sizeof(BLE_packet_t));
  volatile_packet.DTC = "null";

  // init the gps serial command AT communication
 // SerialAT.begin(GPS_baudrate);

  // init the MPU
  Wire.begin();
  initializeMPU9250();
  calibrateMPU9250();
}

void acq_function(int acq_mode)
{
  switch (acq_mode)
  {
    case Accelerometer_ST:      
     imu_acq_function();
     break;

    default:
      Handling_CAN_msg();
      break;
  }
 
}

/*================================ Accelerometer && GPS functions ================================*/
void imu_acq_function()
{
    xyzFloat gValue = MPU9250.getGValues();
    xyzFloat gyr = MPU9250.getGyrValues();
    xyzFloat magValue = MPU9250.getMagValues();
    xyzFloat angles = MPU9250.getAngles();

    //float resultantG = MPU9250.getResultantG(gValue);
    float temp = MPU9250.getTemperature();
    float pitch = MPU9250.getPitch();
    float roll = MPU9250.getRoll();  

    /*BLE PACKET ACC*/
    volatile_packet.imu_acc.acc_x = gValue.x;
    volatile_packet.imu_acc.acc_y = gValue.y;
    volatile_packet.imu_acc.acc_z = gValue.z;

    volatile_packet.imu_ang.ang_x = angles.x;
    volatile_packet.imu_ang.ang_y = angles.y;
    volatile_packet.imu_ang.ang_z = angles.z;

    #ifdef debug_acc
      Serial.print("Acceleration in g (x,y,z):  ");
      Serial.printf("%.2f, ", gValue.x);      
      Serial.printf("%.2f, ", gValue.y);       
      Serial.printf("%.2f\r\n", gValue.z);  
      
      Serial.print("Temperature in °C: ");
      Serial.printf("%.2f\r\n", temp);
      
      Serial.print("Angles X Y Z: ");
      Serial.printf("%.2f, ",angles.x);      
      Serial.printf("%.2f, ",angles.y);       
      Serial.printf("%.2f\r\n",angles.z); 

      Serial.print("Pitch Roll ");
      Serial.printf("%.2f, ",pitch);      
      Serial.printf("%.2f\r\n",roll);
    #endif
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

void initializeMPU9250() {
  // Inicializa o MPU9250
  while (!MPU9250.init()) {
    Serial.println("Acelerometro não responde!");
    delay(1000);
  }
  Serial.println("Acelerometro Inicializado!");

  while (!MPU9250.initMagnetometer()) {
    Serial.println("Magnetometro não responde!");
    delay(1000);
  }
  Serial.println("Magnetometro Inicializado!");

  // Configurações do MPU9250
  MPU9250.enableGyrDLPF();
  MPU9250.setGyrDLPF(MPU9250_DLPF_6);
  MPU9250.setSampleRateDivider(5);
  MPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);
  MPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  MPU9250.enableAccDLPF(true);
  MPU9250.setAccDLPF(MPU9250_DLPF_6);
  MPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
  delay(100);
}
void calibrateMPU9250() {
  Serial.println("Iniciando a calibração do acelerometro...");  
  MPU9250.autoOffsets();
  Serial.println("Feito!");
}