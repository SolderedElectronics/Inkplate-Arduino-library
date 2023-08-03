/**
 *******************************************************************************************
 *
 * @file      serialPlotterExample.ino
 * @brief     An example to see the Gyroscope readings on the Serial plotter!
*             Open Serial plotter at 115200 baud!
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
 * @authors   Robert@Soldered.com
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
    // Get all parameters and print it on the Serial plotter
    // Note: for precise results, you have to calibrate the gyro
    Serial.print("GYROX:");
    Serial.print(myIMU.readFloatAccelX(), 4);
    Serial.print(",");
    Serial.print("GYROY:");
    Serial.print(myIMU.readFloatAccelY(), 4);
    Serial.print(",");
    Serial.print("GYROZ:");
    Serial.println(myIMU.readFloatAccelZ(), 4);

    // Wait a short time because it's the serial plotter
    // Quicker outputs look better
    delay(50);
}
