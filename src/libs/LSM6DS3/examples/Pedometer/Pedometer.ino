/**
 *******************************************************************************************
 *
 * @file      Pedometer.ino
 * @brief     This sketch counts steps taken.
 *            Run the sketch and open a serial window at 115200 baud. The sketch will display
 *            the number of steps taken since reset. lightly tap the sensor on something at the
 *            rate of walking to simulate having the device in your pocket. Note that you must
 *            take 7 regularly spaced steps before the counter starts reporting.
 *
 *            Push the reset button to reset the device and count.
 *
 *            The configuration is determined by reading the LSM6DS3 datasheet and application
 *            note, then driving hex values to the registers of interest to set the appropriate
 *            bits.  The sketch is based of the "LowLevelExampe" sketch.
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

// Create object from LSM library
Soldered_LSM6DS3 myIMU; // Default address is 0x6B

void setup()
{
    // Init serial communication
    Serial.begin(115200);
    delay(1000); // Relax...
    Serial.print("The sketch started - ");

    // Call .beginCore() to configure the IMU
    if (myIMU.beginCore() != 0)
    {
        // Print a message and stop the program if an error occurs
        Serial.print("Error at beginCore().\n");
        while (1)
        {
            delay(100);
        }
    }
    else
    {
        Serial.print("\nbeginCore() passed.\n");
    }

    // Error accumulation variable
    uint8_t errorAccumulator = 0;

    uint8_t dataToWrite = 0; // Temporary variable

    // Setup the accelerometer******************************
    dataToWrite = 0; // Start Fresh!
    //  dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;

    // //Now, write the patched together data
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    // Set the ODR bit
    errorAccumulator += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);


    // Enable embedded functions -- ALSO clears the pdeo step count
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL10_C, 0x3E);
    // Enable pedometer algorithm
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x40);
    // Step Detector interrupt driven to INT1 pin
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT1_CTRL, 0x10);

    if (errorAccumulator)
    {
        // Print a message and stop the program if an error occurs
        Serial.println("Problem configuring the device.");
        while (1)
        {
            delay(100);
        }
    }
    else
    {
        Serial.println("Device O.K.");
    }

    // Wait a bit
    delay(200);
}

void loop()
{
    uint8_t readDataByte = 0;
    uint16_t stepsTaken = 0;

    // Read the 16bit value by two 8bit operations
    myIMU.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_H);
    stepsTaken = ((uint16_t)readDataByte) << 8;

    myIMU.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_L);
    stepsTaken |= readDataByte;

    // Display steps taken
    Serial.print("Steps taken: ");
    Serial.println(stepsTaken);

    // Wait 1 second
    delay(1000);
}
