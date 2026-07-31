/**
 **************************************************
 * @file        Inkplate5_Factory_Programming_VCOM.ino
 *
 * @brief       File for programming the Inkplate's VCOM
 *
 * @note        !WARNING! VCOM can only be set 100 times, so keep usage to a minimum.
 *
 *              !WARNING! This example uses einkOn() and einkOff() methods that turn 
 *                        on power supply for epaper display. They should only be used
 *                        in these examples, otherwise you risk damaging your
 *                        epaper display permanently!
 *
 *              !WARNING! Use at your own risk!!
 *
 *
 *              Inkplate 5 does not support auto VCOM, it has to be set manually.
 *              The user will be prompted to enter VCOM via serial (baud 115200).
 *              VCOM ranges from 0.0 to -5.0.
 *
 *              Tests will also be done, to pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x30 (you may change this in test.cpp also).
 *                In the InkplateEasyCTester folder, you can find the code for uploading to Dasduino Core 
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave device for testing an easyC connector
 *                if you don't have a device with address 0x30.
 *              - Insert a formatted microSD card (doesn't have to be empty)
 *              - Press wake button to finish testing
 *
 * License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 * LICENSE file included with this example. If you have any questions about
 * licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 * warranty is given.
 *
 * Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
 * Looking to get support? Write on our community forum: https://community.soldered.com/
 * 15 April 2024 by Soldered
 *
 * @authors     Soldered
 ***************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include needed libraries in the sketch
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "test.h"
#include "image.h"

// Create object on Inkplate library and set library to work in monochorme mode
Inkplate display(INKPLATE_1BIT);

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMaddress = 1;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

void setup()
{
    Serial.begin(115200);
    display.setTextSize(3);
    EEPROM.begin(512);
    Wire.begin();

    // Wakeup button
    pinMode(GPIO_NUM_36, INPUT);

    // Setting default value for safety
    double vcomVoltage = -1.3;

    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);

    if (isFirstStartup)
    {
        // Try to ping IO expander to test I2C
        Wire.setTimeOut(3000);
        Wire.beginTransmission(IO_INT_ADDR);
        int result = Wire.endTransmission();

        if (result == 5)
        {
            Serial.println("I2C Bus Error!");
            failHandler(true);
        }
    }

    // If I2C is OK, initialise the display
    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        while (true)
        {
          // Get VCOM voltage from serial from user
          uint8_t flag = getVCOMFromSerial(&vcomVoltage);

          // Show the user the entered VCOM voltage
          Serial.print("Entered VCOM: ");
          Serial.println(vcomVoltage);
          display.print(vcomVoltage);
          display.partialUpdate();

          if (display.setVCOM(vcomVoltage))
          {
            Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");

            // Mark the device as tested and provisioned so the tests
            // don't run again on the next boot
            EEPROM.write(EEPROMaddress, 170);
            EEPROM.commit();
            break;
          }
          else
          {
            Serial.println("ERROR");
          }
        }

        display.selectDisplayMode(INKPLATE_3BIT);
    }
    else
    {
        Serial.println("VCOM already set!");
        // *****************************************************
        // Turn on power supply for epaper display. 
        // WARNING: Do not call this method repeatedly as it 
        //          can damage your display if used incorrectly!
        // *****************************************************
        display.einkOn();
    }

    // Clear buffer for peripheral commands
    memset(commandBuffer, 0, BUFFER_SIZE);

    // Print the initial image that remains on the screen
    showSplashScreen(vcomVoltage);
}

void loop()
{
    // Peripheral mode 
    
    if (Serial.available())
    {
        while (Serial.available())
        {
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
            {
                commandBuffer[i] = commandBuffer[i + 1];
            }
            commandBuffer[BUFFER_SIZE - 1] = Serial.read();
        }
    }

    // Function in Peripheral.h
    run(commandBuffer, BUFFER_SIZE, &display);
}

// Print the initial image that remains on the screen
void showSplashScreen(float vComVoltage)
{
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(5, 698);
    display.print(vComVoltage, 2);
    display.print("V");
    display.display();
}

// Prompt user to enter VCOM
double getVCOMFromSerial(double *_vcom)
{
    double vcom = 1;
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Display a message on Inkplate
    display.print("\r\n- Write VCOM on UART: ");
    display.partialUpdate(0, 1);

    while (true)
    {
        Serial.println(
            "Write VCOM voltage from epaper panel.\r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point. "
            "For example -1.23\n");
        while (!Serial.available())
            ;

        serialTimeout = millis();
        int i = 0;
        while ((Serial.available()) && ((unsigned long)(millis() - serialTimeout) < 500))
        {
            if ((Serial.available()) && (i < 49))
            {
                serialBuffer[i++] = Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;
        if (sscanf(serialBuffer, "%lf", &vcom) == 1)
        {
            *_vcom = vcom;
            return 1;
        }
    }

    return 0;
}
