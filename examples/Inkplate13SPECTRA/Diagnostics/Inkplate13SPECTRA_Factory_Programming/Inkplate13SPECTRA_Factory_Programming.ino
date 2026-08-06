/**
 **************************************************
 * @file        Inkplate13SPECTRA_Factory_Programming.ino
 * @brief       File for factory programming Inkplate 13SPECTRA.
 *
 * @details     Runs the factory programming routine together with the hardware
 *              test suite. Output of the tests is done via Serial due to the slow
 *              screen refresh rate.
 *
 *              Tests will also be done. To pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x76 (you may
 *                change this in test.cpp also). In the InkplateEasyCTester
 *                folder, you can find the code for uploading to Dasduino Core
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave
 *                device for testing an easyC connector if you don't have a
 *                device with address 0x76.
 *              - Insert a formatted microSD card (doesn't have to be empty).
 *              - Press wake button to finish testing.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, formatted microSD card,
 *               EasyC slave device on address 0x76
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Edit the WiFi information in test.cpp and connect the EasyC slave device.
 * 3) Insert a formatted microSD card and upload the sketch.
 * 4) Follow the test output on the Serial Monitor.
 * 5) Press the wake button to finish testing.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example. If you
 *              have any questions about licensing, please visit
 *              https://soldered.com/contact/ Distributed as-is; no warranty is
 *              given.
 *
 * @authors     Soldered
 ***************************************************/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Inkplate 13SPECTRA in the boards menu."
#endif

#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

#include "Peripheral.h"
#include "test.h"
#include "image.h"

Inkplate inkplate;

const int EEPROMaddress = 0;

#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

void showSplashScreen();

void setup()
{
    Serial.begin(115200);

    inkplate.setTextSize(4);
    EEPROM.begin(512);
    Wire.begin();

    pinMode(GPIO_NUM_18, INPUT);

    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);

    if (isFirstStartup)
    {
        Wire.setTimeOut(3000);

        Wire.beginTransmission(IO_INT_ADDR);
        if (Wire.endTransmission() != 0)
        {
            Serial.println("I2C Bus error!");
            failHandler();
        }
    }

    inkplate.begin();

    if (isFirstStartup)
    {
        testPeripheral();
        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }

    memset(commandBuffer, 0, sizeof(commandBuffer));

    showSplashScreen();
}

void loop()
{
    if (Serial.available())
    {
        while (Serial.available())
        {
            // sliding window buffer
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
                commandBuffer[i] = commandBuffer[i + 1];

            commandBuffer[BUFFER_SIZE - 1] = (char)Serial.read();
        }
    }

    run(commandBuffer, BUFFER_SIZE, &inkplate);
}

void showSplashScreen()
{
    inkplate.clearDisplay();
    inkplate.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    inkplate.display();
}