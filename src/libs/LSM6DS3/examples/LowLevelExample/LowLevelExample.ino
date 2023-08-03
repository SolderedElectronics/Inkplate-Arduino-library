/**
 *******************************************************************************************
 *
 * @file      lowLevelExample.ino
 * @brief     Example using the LSM6DS3 with ONLY read and write methods. It's up to you to
 *            read the datasheets and get the sensor to behave as you will. This sketch saves 
 *            a significant amount of memory because the settings and complex math (such as 
 *            floating point variables) don't exist.  The cost of saved memory is that the 
 *            values are in 'counts', or raw data from the register. The user is responsible 
 *            for converting these raw values into something meaningful.
 *
 *            Use the register words from SparkFunLSM6DS3.h to manually configure the IC.
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
 * @authors   Marshall Taylor @ SparkFun Electronics, 20 May 2015
 *            Modified by Soldered, 3 April 2023
 *******************************************************************************************/

// Include libraries
#include "LSM6DS3-SOLDERED.h"
#include "Wire.h"

uint16_t errorsAndWarnings = 0;

// Create object from LSM library
Soldered_LSM6DS3 myIMU; // Default address is 0x6B

void setup()
{
    // Put your setup code here, to run once:

    // Init serial communication
    Serial.begin(115200);
    delay(1000); // Relax...
    Serial.print("The sketch started - ");

    // Call .beginCore() to configure the IMU
    if (myIMU.beginCore() != 0)
    {
        Serial.println("Error at beginCore().");
    }
    else
    {
        Serial.println("beginCore() passed.");
    }

    uint8_t dataToWrite = 0; // Temporary variable

    // Setup the accelerometer******************************
    dataToWrite = 0; // Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;

    // Now, write the patched together data
    errorsAndWarnings += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    // Set the ODR bit
    errorsAndWarnings += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);
}


void loop()
{
    int16_t temp;
    // Get all parameters
    Serial.print("\nAccelerometer Counts:\n");
    Serial.print(" X = ");

    // Read a register into the temp variable.
    if (myIMU.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTX_L_XL) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.println(temp);
    Serial.print(" Y = ");

    // Read a register into the temp variable.
    if (myIMU.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTY_L_XL) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.println(temp);
    Serial.print(" Z = ");

    // Read a register into the temp variable.
    if (myIMU.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTZ_L_XL) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.println(temp);

    Serial.println();
    Serial.print("Total reported Errors and Warnings: ");
    Serial.println(errorsAndWarnings);

    delay(1000);
}
