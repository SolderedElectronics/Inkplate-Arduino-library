/**
 *******************************************************************************************
 *
 * @file      memoryPagingExample.ino
 * @brief     This sketch switches between the base memory page and the embedded page.
 *            The test writes to a base address, switches pages, writes to a embedded location
 *            at the same numerical address, switches back and reads the original value.
 *            This sketch doesn't do any meaningful configuration for the LSM6DS3, just tests.
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

    uint8_t dataVariable = 0; // Temporary variable


    // Write something to a base page location to make sure it gets preserved
    // Then, read it back
    if (myIMU.writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL1, 0xF0) != 0)
    {
        errorsAndWarnings++;
    }
    if (myIMU.readRegister(&dataVariable, LSM6DS3_ACC_GYRO_FIFO_CTRL1) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.print("FIFO_CTRL1 (should read 0xF0): 0x");
    Serial.println(dataVariable, HEX);


    // Switch to the embedded page
    if (myIMU.embeddedPage() != 0)
    {
        errorsAndWarnings++;
    }

    // Write something to a the embedded page at the same address
    // Then, read it back
    if (myIMU.writeRegister(LSM6DS3_ACC_GYRO_SLV1_SUBADD, 0xA5) != 0)
    {
        errorsAndWarnings++;
    }
    // Now read it back and display it
    if (myIMU.readRegister(&dataVariable, LSM6DS3_ACC_GYRO_SLV1_SUBADD) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.print("SUBADD (should read 0xA5): 0x");
    Serial.println(dataVariable, HEX);

    // Switch back to the base page
    // Then, read back to see if our value has been preserved
    if (myIMU.basePage() != 0)
    {
        errorsAndWarnings++;
    }
    if (myIMU.readRegister(&dataVariable, LSM6DS3_ACC_GYRO_FIFO_CTRL1) != 0)
    {
        errorsAndWarnings++;
    }
    Serial.print("FIFO_CTRL1 (should read 0xF0): 0x");
    Serial.println(dataVariable, HEX);

    Serial.print("Number of errors: ");
    Serial.println(errorsAndWarnings);
}


void loop()
{
    // Nothing...
}
