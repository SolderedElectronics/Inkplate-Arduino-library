/**
 *******************************************************************************************
 *
 * @file      interruptFreeFallConfig.ino
 * @brief     This sketch demonstrates detecting the free-fall condition. Run the sketch,
 *            open the Serial Monitor at 115200 baud, then drop your sensor / Dasduino
 *            (And catch it!). The sketch will report the free-fall to the monitor.
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
volatile bool int1Status = false;

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
        Serial.println("beginCore() passed.");
    }

    // Error accumulation variable
    uint8_t errorAccumulator = 0;

    uint8_t dataToWrite = 0; // Temporary variable

    // Setup the accelerometer******************************
    dataToWrite = 0; // Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

    // //Now, write the patched together data
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
    errorAccumulator += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);

    // Write 00h into WAKE_UP_DUR
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_DUR, 0x00);

    // Set FF threshold (FF_THS[2:0] = 011b)
    // Set six samples event duration (FF_DUR[5:0] = 000110b)
    // Write 33h into FREE_FALL
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_FREE_FALL, 0x33);

    // FF interrupt driven to INT1 pin
    // Write 10h into MD1_CFG
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x10);
    // Also route to INT2 pin
    // Write 10h into MD1_CFG
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD2_CFG, 0x10);

    // Latch interrupt
    // Write 01h into TAP_CFG
    errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x01);

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
        Serial.println("Now you can drop the sensor (And catch it!)");
    }

    // Configure the atmega interrupt pin
    pinMode(int1Pin, INPUT);
    attachInterrupt(0, int1ISR, RISING);
}


void loop()
{
    if (int1Status)
    {
        int1Status = false;

        uint8_t readDataByte = 0;

        // Read the wake-up source register
        myIMU.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_WAKE_UP_SRC);

        // Mask off the FF_IA bit for free-fall detection
        readDataByte &= 0x20;

        // Check for free-fall
        if (readDataByte)
        {
            // debounce
            delay(10);
            Serial.println("Interrupt caught, free fall detected!");
        }
    }
}

// Interrupt service routine
void int1ISR()
{
    int1Status = true;
}