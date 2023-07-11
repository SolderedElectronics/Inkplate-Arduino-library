/**
 *******************************************************************************************
 *
 * @file      calibrateGyro.ino
 * @brief     This example will show you how to calibrate the gyroscope.
 * 
 *            IMPORTANT: When the gyroscope is calibrating, don't move it!
 *            You can change the number of samples for calibration.
 *
 *            Hardware connections:
 *            Connect SDA to A4
 *            Connect SCL to A5
 *            Connect GND and 3V3 or 5V power to the IMU
 *            Or simply use easyC cable
 *
 *            Resources:
 *            Uses Wire.h for i2c operation
 *
 *            Development environment specifics:
 *            Arduino IDE 1.8.13
 *
 *            Distributed as-is; no warranty is given.
 *
 * @link      solde.red/333069
 *
 * @author    Karlo Leksic for Soldered, 5 April 2023
 *******************************************************************************************/

// Include libraries
#include "LSM6DS3-SOLDERED.h"
#include "Wire.h"

// Create object from LSM library
Soldered_LSM6DS3 myIMU;

float gyro_x_call, gyro_y_call, gyro_z_call;

void setup()
{
    // Init serial communication
    Serial.begin(115200);
    delay(1000); // Relax...
    Serial.print("The sketch started - ");

    // Call .begin() to configure the IMU
    myIMU.begin();

    // Let's calibrate the gyro
    // IMPORTANT: When the gyroscope is calibrating, don't move it!
    Serial.print("Calibrating - Don't move gyro!");

    // If you want, you can change the number of calibration samples
    int numSamples = 2000;

    // Read samples and calculate the average value for each axis
    for(int i = 0; i < numSamples; i++)
    {
      // Print a dot on the serial sometimes
      if(i % 200 == 0)
      {
        Serial.print(".");
      }

      // Add values to calculate the average later
      gyro_x_call += myIMU.readFloatGyroX();
      gyro_y_call += myIMU.readFloatGyroY();
      gyro_z_call += myIMU.readFloatGyroZ();
      
      delay(3);
    }

    // Calculate average
    gyro_x_call /= numSamples;
    gyro_y_call /= numSamples;
    gyro_z_call /= numSamples;

    Serial.println();
}

void loop()
{
    // Print calibrated rotational velocity
    Serial.print("Gyroscope:");
    Serial.print(" X = ");
    // Just subtract the average value (calibrated one) and it should be around 0 when it's not moving
    Serial.print(myIMU.readFloatGyroX() - gyro_x_call, 1); 
    Serial.print(" Y = ");
    Serial.print(myIMU.readFloatGyroY() - gyro_y_call, 1);
    Serial.print(" Z = ");
    Serial.println(myIMU.readFloatGyroZ() - gyro_z_call, 1);

    // Relax...
    delay(200);
}
