/**
 *******************************************************************************************
 *
 * @file      minimalistExample.ino
 * @brief     Most basic example of use. Example using the LSM6DS3 with basic settings.  
 *            This sketch collects Gyro and Accelerometer data every second, then presents 
 *            it on the Serial Monitor.
 *
 *            Hardware connections:
 *            Connect SDA to A4
 *            Connect SCL to A5
 *            Connect GND and 3V3 or 5V power to the breakout.
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
 * @authors   Owen Lyke @ SparkFun Electronics, 13 March 2019
 *            Modified by Soldered, 3 April 2023
 *******************************************************************************************/

// Include libraries
#include "LSM6DS3-SOLDERED.h"
#include "Wire.h"

// Create object from LSM library
Soldered_LSM6DS3 myIMU; // Default address is 0x6B

void setup()
{
    // Init serial communication
    Serial.begin(115200);
    delay(1000); // Relax...

    // Call .begin() to configure the IMU
    myIMU.begin();
}

void loop()
{
    // Get all parameters and print it on the Serial Monitor

    // Read acceleration and print it on serial
    Serial.print("ACCX:");
    Serial.print(myIMU.readFloatAccelX(), 4);
    Serial.print(",");
    Serial.print("ACCY:");
    Serial.print(myIMU.readFloatAccelY(), 4);
    Serial.print(",");
    Serial.print("ACCZ:");
    Serial.print(myIMU.readFloatAccelZ(), 4);
    Serial.print(",");

    // Read rotational velocity and print it on serial
    // Note: for precise results, you have to calibrate the gyro
    Serial.print("GYROX:");
    Serial.print(myIMU.readFloatGyroX(), 4);
    Serial.print(",");
    Serial.print("GYROY:");
    Serial.print(myIMU.readFloatGyroY(), 4);
    Serial.print(",");
    Serial.print("GYROZ:");
    Serial.print(myIMU.readFloatGyroZ(), 4);
    Serial.print(",");

    // Read the temperature and print it on serial
    Serial.print("DEGC:");
    Serial.print(myIMU.readTempC(), 4);
    Serial.print(",");
    Serial.print("DEGF:");
    Serial.println(myIMU.readTempF(), 4);

    // Relax...
    delay(150);
}
