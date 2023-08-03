/**
 **************************************************
 * @file        Inkplate6COLOR_Factory_Programming.ino
 *
 * @brief       File for factory programming Inkplate6 COLOR
 *
 * @note        Tests will also be done, to pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x30 (you may change this in test.cpp also).
 *                In the InkplateEasyCTester folder, you can find the code for uploading to Dasduino Core 
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave device for testing an easyC connector
 *                if you don't have a device with address 0x30.
 *              - Insert a formatted microSD card (doesn't have to be empty)
 *              - Press wake button to finish testing
 *              Output of the tests will be done via Serial due to slow screen refresh rate.
 *
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATECOLORV2)
#error "Wrong board selection for this example, please select Inkplate 6 Color in the boards menu."
#endif

// Do not forget to add WiFi SSID and WiFi Password in test.cpp!

// Include needed libraries in the sketch
#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "test.h"
#include "image.h"

Inkplate display;

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMaddress = 0;

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

    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);

    if (isFirstStartup)
    {
        Wire.setTimeOut(3000);
        // Try to ping IO expander to test I2C
        Wire.beginTransmission(IO_INT_ADDR);
        if (Wire.endTransmission() != 0)
        {
            Serial.println("I2C Bus error!");
            failHandler();
        }
    }

    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }

    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen();
}

void loop()
{
    // Peripheral mode 
    // More about peripheral mode: https://inkplate.readthedocs.io/en/latest/peripheral-mode.html

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
void showSplashScreen()
{
    display.clearDisplay();
    display.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.display();
}
