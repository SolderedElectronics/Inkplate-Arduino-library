/**
 *******************************************************************************************
 *
 * @file      multiI2C.ino
 * @brief     Example using up to two LSM6DS3s on the same I2C channel. If only one sensor
 *            is attached, this sketch reports failure on that channel and runs with the
 *            single sensor instead.
 *
 *            Hardware connections:
 *            Connect SDA to A4
 *            Connect SCL to A5
 *            Connect GND and 3V3 or 5V power to the breakout.
 *            Or simply use easyC cable
 *
 *            NOTE: Multiple I2C devices use the same pins. Up to two LSM6DS3s are allowed.
 *                  Use the solder jumper to select address 0x6A or 0x6B
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
 * @authors   Marshall Taylor @ SparkFun Electronics, 20 May 2015
 *            Modified by Soldered, 3 April 2023
 *******************************************************************************************/

// Include libraries
#include "LSM6DS3-SOLDERED.h"
#include "Wire.h"

// Create two instances of the driver class
Soldered_LSM6DS3 SensorOne(0x6A);
Soldered_LSM6DS3 SensorTwo(0x6B);

void setup()
{
    // Init serial communication
    Serial.begin(115200);
    delay(1000); // Relax...
    Serial.println("The sketch started - ");

    // Call .begin() to configure each IMU
    // Sensor 1
    if (SensorOne.begin() != 0)
    {
        Serial.println("Problem starting the sensor at 0x6A.");
    }
    else
    {
        Serial.println("Sensor at 0x6A started.");
    }

    // Sensor 2
    if (SensorTwo.begin() != 0)
    {
        Serial.println("Problem starting the sensor at 0x6B.");
    }
    else
    {
        Serial.println("Sensor at 0x6B started.");
    }
}

void loop()
{
    // Get all parameters and print it on the Serial Monitor

    // Read acceleration for sensor one and print it on serial
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X1 = ");
    Serial.println(SensorOne.readFloatAccelX(), 4);
    Serial.print(" Y1 = ");
    Serial.println(SensorOne.readFloatAccelY(), 4);
    Serial.print(" Z1 = ");
    Serial.println(SensorOne.readFloatAccelZ(), 4);

    // Read acceleration for sensor two and print it on serial
    Serial.print(" X2 = ");
    Serial.println(SensorTwo.readFloatAccelX(), 4);
    Serial.print(" Y2 = ");
    Serial.println(SensorTwo.readFloatAccelY(), 4);
    Serial.print(" Z2 = ");
    Serial.println(SensorTwo.readFloatAccelZ(), 4);


    // Read rotational velocity for sensor one and print it on serial
    // Note: for precise results, you have to calibrate the gyro
    Serial.print("\nGyroscope:\n");
    Serial.print(" X1 = ");
    Serial.println(SensorOne.readFloatGyroX(), 4);
    Serial.print(" Y1 = ");
    Serial.println(SensorOne.readFloatGyroY(), 4);
    Serial.print(" Z1 = ");
    Serial.println(SensorOne.readFloatGyroZ(), 4);

        // Read rotational velocity for sensor two and print it on serial
    // Note: for precise results, you have to calibrate the gyro
    Serial.print(" X2 = ");
    Serial.println(SensorTwo.readFloatGyroX(), 4);
    Serial.print(" Y2 = ");
    Serial.println(SensorTwo.readFloatGyroY(), 4);
    Serial.print(" Z2 = ");
    Serial.println(SensorTwo.readFloatGyroZ(), 4);


    // Read the temperature from sensor one and print it on serial
    Serial.print("\nThermometer:\n");
    Serial.print(" Degrees C1 = ");
    Serial.println(SensorOne.readTempC(), 4);
    Serial.print(" Degrees C2 = ");
    Serial.println(SensorTwo.readTempC(), 4);

    // Read the temperature from sensor two and print it on serial
    Serial.print(" Degrees F1 = ");
    Serial.println(SensorOne.readTempF(), 4);
    Serial.print(" Degrees F2 = ");
    Serial.println(SensorTwo.readTempF(), 4);


    // Print errors
    Serial.print("\nSensorOne Bus Errors Reported:\n");
    Serial.print(" All '1's = ");
    Serial.println(SensorOne.allOnesCounter);
    Serial.print(" Non-success = ");
    Serial.println(SensorOne.nonSuccessCounter);
    Serial.print("SensorTwo Bus Errors Reported:\n");
    Serial.print(" All '1's = ");
    Serial.println(SensorTwo.allOnesCounter);
    Serial.print(" Non-success = ");
    Serial.println(SensorTwo.nonSuccessCounter);

    // Relax...
    delay(1000);
}
