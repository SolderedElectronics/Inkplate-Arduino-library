/**
 *******************************************************************************************
 *
 * @file      interruptHWTapConfig.ino
 * @brief     Example using the LSM6DS3 interrupts. This sketch demonstrates one way to
 *            detect single and double-tap events using hardware interrupt pins. The LSM6DS3
 *            pulses the int1 line once after the first tap, then again if a second tap comes in.
 *
 *            The configuration is determined by reading the LSM6DS3 datasheet and application
 *            note, then driving hex values to the registers of interest to set the appropriate
 *            bits. The sketch is based off the "LowLevelExample" sketch.
 *
 *            Hardware connections:
 *            Connect SDA to A4
 *            Connect SCL to A5
 *            Connect GND and 3V3 or 5V power to the breakout.
 *            Or simply use easyC cable.
 *            Additionally, Connect INT1 to pin 2. You can adjust this depending on which
 *            interrupt pins are on the board is used. This is for Dasduino Core.
 *
 *            NOTE: Do not configure pin 2 as OUTPUT!
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

// Interrupt variables
#define int1Pin 2 // Use pin 2 for interrupt. Change if you are using a board with other interrupt pins
uint8_t int1Status = 0;

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
        Serial.println("Error at beginCore().");
        while (1)
        {
            // Nothing...
        }
    }
    else
    {
        Serial.println("beginCore() passed.");
    }

    // Error accumulation variable
    uint8_t errorAccumulator = 0;

    uint8_t dataToWrite = 0; // Temporary variable

    // Setup the accelerometer
    dataToWrite = 0; // Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

    // //Now, write the patched together data
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    // Set the ODR bit
    errorAccumulator += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

    // Enable tap detection on X, Y, Z axis, but do not latch output

    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E);

    // Set tap threshold
    // Write 0Ch into TAP_THS_6D
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x03);

    // Set Duration, Quiet and Shock time windows
    // Write 7Fh into INT_DUR2
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);

    // Single & Double tap enabled (SINGLE_DOUBLE_TAP = 1)
    // Write 80h into WAKE_UP_THS
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);

    // Single tap interrupt driven to INT1 pin -- enable latch
    // Write 08h into MD1_CFG
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x48);

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

    // Configure the atmega interrupt pin
    pinMode(int1Pin, INPUT);
    attachInterrupt(0, int1ISR, RISING);
}


void loop()
{
    // If ISR has been serviced at least once
    if (int1Status > 0)
    {
        // Wait for a window (in case a second tap is coming)
        delay(300);

        // Check if there are more than one interrupt pulse
        if (int1Status == 1)
        {
            Serial.println("First-tap event");
        }
        if (int1Status > 1)
        {
            Serial.println("Second-tap event");
        }

        // Clear the ISR counter
        int1Status = 0;
    }
}

// Interrupt service routine
void int1ISR()
{
    int1Status++;
}
