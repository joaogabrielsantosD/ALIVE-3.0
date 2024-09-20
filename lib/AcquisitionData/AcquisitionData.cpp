#include "AcquisitionData.h"

BLE_packet_t volatile_packet;

/* Module variables */
TinyGPSPlus NEO_M8N;
MPU9250_WE MPU9250 = MPU9250_WE(MPU9250_ADDR);

/* Debug Variables */
//#define debug_when_receive_byte  
//#define debug_acc             //Print on Serial  paramenters
//#define debug_GPS             //Print on Serial the paramenters
//#define debug_when_send // Variable to print the messageStructure when sended to Car, Uncomment to Enable.

void start_module_device()
{
  memset(&volatile_packet, 0, sizeof(BLE_packet_t));
  volatile_packet.DTC = "null";

  // init the gps serial communication with GPS module
  SerialGPS.begin(GPSBaudRate);

  // init the MPU
  Wire.begin();

  if (MPU9250.init() && MPU9250.initMagnetometer())
  {
    #ifdef debug_acc
      Serial.println("Accelerometer and Magnetometer Initialized");
    #endif

    // MPU9250 settings
    MPU9250.setSampleRateDivider(5);
    MPU9250.autoOffsets();
    MPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
    vTaskDelay(100);
    // save_flag_imu_parameter(true);
  }
  
  #ifdef debug_acc
    else
      Serial.println("Accelerometer and Magnetometer not responding!!");
  #endif
}



void cleanDTC()
{
  volatile_packet.DTC.clear();
  volatile_packet.DTC = "null";
}

/*================================ Accelerometer && GPS functions ================================*/
void imu_acq_function()
{
  xyzFloat gValue = MPU9250.getGValues();
  //xyzFloat gyr = MPU9250.getGyrValues();
  //xyzFloat magValue = MPU9250.getMagValues();
  xyzFloat angles = MPU9250.getAngles();

  //float resultantG = MPU9250.getResultantG(gValue); 

  /* BLE PACKET ACC */
  volatile_packet.imu_acc.acc_x = gValue.x;
  volatile_packet.imu_acc.acc_y = gValue.y;
  volatile_packet.imu_acc.acc_z = gValue.z;

  volatile_packet.imu_ang.ang_x = angles.x;
  volatile_packet.imu_ang.ang_y = angles.y;
  volatile_packet.imu_ang.ang_z = angles.z;

  volatile_packet.acctemp = MPU9250.getTemperature();

  #ifdef debug_acc
    Serial.print("Acceleration in g (x,y,z):  ");
    Serial.printf("%.2f, ", gValue.x);      
    Serial.printf("%.2f, ", gValue.y);       
    Serial.printf("%.2f\r\n", gValue.z);  
    
    Serial.print("Temperature in °C: ");
    Serial.printf("%.2f\r\n", volatile_packet.acctemp);
    
    Serial.print("Angles X Y Z: ");
    Serial.printf("%.2f, ",angles.x);      
    Serial.printf("%.2f, ",angles.y);       
    Serial.printf("%.2f\r\n",angles.z); 

    Serial.print("Pitch Roll ");
    Serial.printf("%.2f, ", MPU9250.getPitch());      
    Serial.printf("%.2f\r\n", MPU9250.getRoll());
  #endif
}   

void gps_acq_function()
{
  static bool first_conection = false;

  if (NEO_M8N.satellites.isValid() && NEO_M8N.location.isValid())
  {
    volatile_packet.gps_data.LAT = NEO_M8N.location.lat();
    volatile_packet.gps_data.LNG = NEO_M8N.location.lng();

    #ifdef debug_GPS
      Serial.printf("Satellites: %d\r\n", NEO_M8N.satellites.value());
      Serial.printf("Latitude: %lf\r\n", volatile_packet.gps_data.LAT);
      Serial.printf("Longitude: %lf\r\n", volatile_packet.gps_data.LNG);      
    #endif
    
    if (!first_conection)
    {
      first_conection = true;
      // save_flag_gps_parameter(true);
    }
  }

  // This ensures that the gps object is being "fed".
  while (SerialGPS.available() > 0)
    NEO_M8N.encode(SerialGPS.read());
}


