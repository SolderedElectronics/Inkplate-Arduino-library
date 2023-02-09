/*
   Inkplate6COLOR_EEPROM_Usage example for Soldered Inkplate 6COLOR
   For this example, you will need only USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to use EEPROM with Inkplate board.
   EEPROM is a permanent memory that holds data even if the power supply is disconnected.
   You can use EEPROM to store any data you don't want to lose during restarting or powering down the device.
   It shows how to use basic operations with EEPROM like clearing, writing, and reading.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   5 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_SIZE 128 // How much data to write to EEPROM in this example

Inkplate display; // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    Serial.begin(115200);                 // Init serial monitor to display what's happening
    display.begin();                      // Init library (you should call this function ONLY ONCE)
    display.setTextColor(INKPLATE_BLACK); // Set text color to black

    // Init EEPROM library with 128 of EEPROM size.
    EEPROM.begin(EEPROM_SIZE);

    display.setTextSize(4);               // Set text size
    Serial.println("Cleaning EEPROM..."); // Print message on serial monitor
    clearEEPROM();                        // Clear user EEPROM data
    delay(500);                           // Wait a little bit...

    Serial.println("Writing data to EEPROM..."); // Print message on serial monitor
    writeEEPROM();                               // Write some data to EEPROM
    delay(500);                                  // Wait a little bit...

    display.println("Reading data from EEPROM:\n"); // Print message on the screen
    Serial.println("Reading data from EEPROM...");  // Print message on the Sserial Monitor
    display.setTextSize(2);                         // Use smaller text so everything can fit on display
    printEEPROM();                                  // Read data from EEPROM and display it on screen
    delay(500);                                     // Wait a little bit...
}

void loop()
{
    // Empty...
}

// Function for clearing EEPROM data
void clearEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, i); 
    }
    EEPROM.commit();
}

// Function reads back previously written data and displays it on serial monitor and screen.
void printEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        display.print(EEPROM.read(i), DEC);
        Serial.print(EEPROM.read(i), DEC);
        if (i != EEPROM_SIZE - 1)
        {
            display.print(", ");
            Serial.print(", ");
        }
    }
    display.display();
}
