/**
 **************************************************
 * @file        Inkplate10_Factory_Programming_VCOM.ino
 *
 * @brief       File for programming the Inkplate's VCOM
 *
 * @note        Tests will also be done, to pass all tests:
 *              - edit the WiFi information in test.cpp.
 *              - connect a slave device via EasyC on address 0x30 (you may change this in test.cpp also).
 *
 * License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 * LICENSE file included with this example. If you have any questions about
 * licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 * warranty is given.
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

// Include files for images and test functions
#include "images.h"
#include "test.h"

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
        if (i > 2)
        {
            i = 0;
        }

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
    switch (i)
    {
    case 0:
        display.drawImage(image1, 0, 0, 212, 104);
        break;

    case 1:
        display.drawImage(image2, 0, 0, 212, 104);
        break;

    case 2:
        display.drawImage(image3, 0, 0, 212, 104);
        break;
    }
    display.display();


    // Go to the sleep
    esp_deep_sleep_start();
}

void loop()
{ 
    // Nothing ...
}
