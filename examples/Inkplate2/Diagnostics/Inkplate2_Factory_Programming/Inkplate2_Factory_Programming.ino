/**
 **************************************************
 * @file        Inkplate2_Factory_Programming.ino
 * @brief       File for factory programming Inkplate 2.
 *
 * @details     Runs the factory programming routine together with the hardware
 *              test suite. Output of the tests is done via Serial due to the slow
 *              screen refresh rate.
 *
 *              Tests will also be done. To pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x30 (you may
 *                change this in test.cpp also). In the InkplateEasyCTester
 *                folder, you can find the code for uploading to Dasduino Core
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave
 *                device for testing an easyC connector if you don't have a
 *                device with address 0x30.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable, EasyC slave device on address 0x30
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Edit the WiFi information in test.cpp and connect the EasyC slave device.
 * 3) Upload the sketch to Inkplate 2.
 * 4) Follow the test output on the Serial Monitor.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
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

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// Include needed libraries
#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

// Include files for test functions
#include "test.h"

// Include .h files of 4 pictures. All four pictures were converted using Inkplate Image Converter
#include "splashscreen.h"
#include "picture1.h"
#include "picture2.h"
#include "picture3.h"

// This array of pointers holds address of every picture in the memory, so we can easly select it by selecting index in
// array
const uint8_t *pictures[] = {inkplate_2_demo_image, picture1, picture2, picture3};

// An object of the Inkplate library
Inkplate display;

// Define EEPROM address for a flag that indicates the first startup and its value
#define EEPROM_ADDRESS 0
#define EEPROM_FLAG    170

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init EEPROM
    EEPROM.begin(512);

    // Init I2C and set the timeout
    Wire.begin();
    Wire.setTimeOut(1000);

    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Check for the first run of this code
    bool isFirstStartup = (EEPROM.read(EEPROM_ADDRESS) != EEPROM_FLAG);

    // If it is the first startup
    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        // Write the flag into the EEPROM to know next time that it isn't the first startup
        EEPROM.write(EEPROM_ADDRESS, EEPROM_FLAG);

        // On the byte after the flag write an index of the first image to display.
        EEPROM.write(EEPROM_ADDRESS + 1, 0);
        EEPROM.commit();
    }
    else
    {
        // Read the image index which is last displayed
        int i = EEPROM.read(EEPROM_ADDRESS + 1);

        // Increment the index
        i++;

        // We do not have more than 4 images, so roll back to zero
        if (i > 3)
            i = 0;

        // Write the new image index for displaying
        EEPROM.write(EEPROM_ADDRESS + 1, i);
        EEPROM.commit();
    }

    // Display the image on the index written in the EEPROM on the address after the flag
    // Read the image index
    int i = EEPROM.read(EEPROM_ADDRESS + 1);

    // Clear the display
    display.clearDisplay();

    // Draw the image on the screen
    display.image.draw(pictures[i], 0, 0, 212, 104);
    display.display();

    // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Nothing ...
}
