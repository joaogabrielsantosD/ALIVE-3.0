#include "AcquisitionData.h"

/* Module variables */
TinyGPSPlus NEO_M8N;
MPU9250_WE MPU9250 = MPU9250_WE(MPU9250_ADDR);

bool init_ACC = false;

/* Debug Variables */
//#define debug_acc             // Print on Serial ACC data
//#define debug_GPS             // Print on Serial GPS data

void start_module_device()
{
  // Init the gps serial communication with GPS module
  SerialGPS.begin(GPSBaudRate);

  // Init the MPU
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
    init_ACC = true;
  }
  
  #ifdef debug_acc
    else
      Serial.println("Accelerometer and Magnetometer not responding!!");
  #endif
}

/*================================ Accelerometer && GPS functions ================================*/
void imu_acq_function(BLE_packet_t *packet)
{
  if (init_ACC)
  {
    xyzFloat gValue = MPU9250.getGValues();
    //xyzFloat gyr = MPU9250.getGyrValues();
    //xyzFloat magValue = MPU9250.getMagValues();
    xyzFloat angles = MPU9250.getAngles();

    //float resultantG = MPU9250.getResultantG(gValue); 

    packet->imu_acc.acc_x = gValue.x;
    packet->imu_acc.acc_y = gValue.y;
    packet->imu_acc.acc_z = gValue.z;

    packet->imu_ang.ang_x = angles.x;
    packet->imu_ang.ang_y = angles.y;
    packet->imu_ang.ang_z = angles.z;

    packet->acctemp = MPU9250.getTemperature();

    #ifdef debug_acc
      Serial.print("\r\nAcceleration in g (x,y,z):  ");
      Serial.printf("%.2f, ", packet->imu_acc.acc_x);      
      Serial.printf("%.2f, ", packet->imu_acc.acc_y);       
      Serial.printf("%.2f\r\n", packet->imu_acc.acc_z);  
      
      Serial.print("Temperature in °C: ");
      Serial.printf("%.2f\r\n", packet->acctemp);
      
      Serial.print("Angles X Y Z: ");
      Serial.printf("%.2f, ", packet->imu_ang.ang_x);      
      Serial.printf("%.2f, ", packet->imu_ang.ang_y);       
      Serial.printf("%.2f\r\n", packet->imu_ang.ang_z); 

      Serial.print("Pitch Roll ");
      Serial.printf("%.2f, ", MPU9250.getPitch());      
      Serial.printf("%.2f\r\n", MPU9250.getRoll());
    #endif
  }
}   

void gps_acq_function(BLE_packet_t *packet)
{
  if (SerialGPS.available() > 0)  
  {
    // This ensures that the gps object is being "fed".
    while (SerialGPS.available() > 0)
      NEO_M8N.encode(SerialGPS.read());

    if (NEO_M8N.satellites.isUpdated() || NEO_M8N.location.isUpdated())
    {
      packet->gps_data.LAT = NEO_M8N.location.lat();
      packet->gps_data.LNG = NEO_M8N.location.lng();

      #ifdef debug_GPS
        Serial.printf("\r\nSatellites: %d\r\n", NEO_M8N.satellites.value());
        Serial.printf("Latitude: %lf\r\n", packet->gps_data.LAT);
        Serial.printf("Longitude: %lf\r\n", packet->gps_data.LNG);      
      #endif
    }
  }
}
