/*
   Inkplate4TEMPERA_EEPROM_Usage example for Soldered Inkplate 4TEMPERA
   For this example, you will need only a USB-C cable and Inkplate 4TEMPERA.
   Select "Soldered Inkplate4TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate4TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to use EEPROM with Inkplate board.
   EEPROM is a permanent memory that holds data even if the power supply is disconnected.
   You can use EEPROM to store any data you don't want to lose during restarting or powering down the device.
   It shows how to use basic operations with EEPROM like clearing, writing, and reading.

   On the Serial Monitor at 115200 baud rate you can see what's happening.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   26 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Soldered Inkplate4TEMPERA in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_SIZE 128 // How much data to write to EEPROM in this example

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Init library (you should call this function ONLY ONCE)
    display.begin();

    // Init serial communication
    Serial.begin(115200);

    // Init EEPROM library with 128 of EEPROM size.
    EEPROM.begin(EEPROM_SIZE);

    display.setTextSize(3);                  // Set text size
    display.setCursor(10, 10);
    display.println("Clearing EEPROM...\n"); // Print message
    Serial.println("Clearing EEPROM...\n");  // Print message
    clearEEPROM();                           // Clear user EEPROM data
    delay(500);                              // Wait a little bit...

    display.setCursor(10, 50);
    display.println("Writing data to EEPROM...\n"); // Print message
    Serial.println("Writing data to EEPROM...\n");  // Print message
    writeEEPROM();                                  // Write some data to EEPROM
    delay(500);                                     // Wait a little bit...

    display.setCursor(10, 90);
    display.println("Reading data from EEPROM:\n"); // Print message
    Serial.println("Reading data from EEPROM:\n");  // Print message
    display.setTextSize(2);                         // Use smaller text so everything can fit on display
    printEEPROM();                                  // Read data from EEPROM and display it on screen
    display.display();
}

void loop()
{
    // Empty...
}

// Function for clearing EEPROM data
void clearEEPROM()
{
    // Go through each address and clear it (write 0)
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
    // Go through each address and write current index
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, i);
    }
    EEPROM.commit();
}

// Function reads back previously written data and displays it on screen
void printEEPROM()
{
    // Go through each address and read a value from it
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        // Print read value in decimal
        display.print(EEPROM.read(i), DEC);
        Serial.print(EEPROM.read(i));

        // Print a comma after each number except the last one
        if (i != EEPROM_SIZE - 1)
        {
            display.print(", ");
            Serial.print(", ");
        }
    }
}
