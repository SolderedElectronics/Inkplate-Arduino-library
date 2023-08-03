/**
 *******************************************************************************************
 *
 * @file      doubleCheckSettings.ino
 * @brief     Example using the LSM6DS3 with basic settings. This sketch collects Gyro and
 *            Accelerometer data every second, then presents it on the Serial Monitor
 *            at 115200 baud rate.
 *
 *            Hardware connections:
 *            Connect SDA to A4
 *            Connect SCL to A5
 *            Connect GND and 3V3 or 5V power to the breakout.
 *            Or simply use easyC cable.
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
    Serial.print("The sketch started - ");

    // Let's choose an unsupported setting...
    myIMU.settings.accelSampleRate = 404; // Typo, 'meant' to type '104'

    // Make a SensorSettings object to remember what you wanted to set everyhting to
    SensorSettings settingsIWanted;

    // Call .begin() to configure the IMU - supplying pointer to the SensorSettings structure
    myIMU.begin(&settingsIWanted);

    // Compare the sensor settings structure to know if anything was changed
    compareSettings(settingsIWanted);
}


void loop()
{
    // Get all parameters and print it on the Serial Monitor

    // Read acceleration and print it on serial
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(myIMU.readFloatAccelX(), 4);
    Serial.print(" Y = ");
    Serial.println(myIMU.readFloatAccelY(), 4);
    Serial.print(" Z = ");
    Serial.println(myIMU.readFloatAccelZ(), 4);

    // Read rotational velocity and print it on serial
    // Note: for precise results, you have to calibrate the gyro
    Serial.print("\nGyroscope:\n");
    Serial.print(" X = ");
    Serial.println(myIMU.readFloatGyroX(), 4);
    Serial.print(" Y = ");
    Serial.println(myIMU.readFloatGyroY(), 4);
    Serial.print(" Z = ");
    Serial.println(myIMU.readFloatGyroZ(), 4);

    // Read the temperature and print it on serial
    Serial.print("\nThermometer:\n");
    Serial.print(" Degrees C = ");
    Serial.println(myIMU.readTempC(), 4);
    Serial.print(" Degrees F = ");
    Serial.println(myIMU.readTempF(), 4);

    // Relax...
    delay(1000);
}

// Compare settings and print a message if the settings were changed
void compareSettings(SensorSettings desiredSettings)
{
    if (myIMU.settings.accelBandWidth != desiredSettings.accelBandWidth)
    {
        Serial.println("'accelBandWidth' was changed!");
    }

    if (myIMU.settings.accelRange != desiredSettings.accelRange)
    {
        Serial.println("'accelRange' was changed!");
    }

    if (myIMU.settings.accelSampleRate != desiredSettings.accelSampleRate)
    {
        Serial.println("'accelSampleRate' was changed!");
    }

    if (myIMU.settings.gyroRange != desiredSettings.gyroRange)
    {
        Serial.println("'gyroRange' was changed!");
    }

    if (myIMU.settings.gyroSampleRate != desiredSettings.gyroSampleRate)
    {
        Serial.println("'gyroSampleRate' was changed!");
    }

    // That's all the settings that might get changed in 'begin()'
}
