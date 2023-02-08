/*
   Inkplate_EEPROM_Usage example for Soldered Inkplate 6
   For this example, you will need only USB cable and Inkplate 6.
   Select "e-radionica Inkplate6" or "Soldered Inkplate6" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to use EEPROM with Inkplate board.
   EEPROM is a permanent memory that holds data even if the power supply is disconnected. 
   You can use EEPROM to store any data you don't want to lose during restarting or powering down the device.
   It shows how to use basic operations with EEPROM like clearing, writing, and reading.
   CAUTION! Changing EEPROM size can wipe waveform data.
   CAUTION! EEPROM addresses from 0 to 75 are used for waveform. DO NOT WRITE OR MODIFY DATA ON THESE ADDRESSES!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   5 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_START_ADDR 76  // Start EEPROM address for user data. Addresses below address 76 are waveform data!
#define EEPROM_SIZE       128 // How much data to write to EEPROM in this example

Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    display.begin(); // Init library (you should call this function ONLY ONCE)

    // Init EEPROM library with 512 of EEPROM size. Do not change this value, it can wipe waveform data!
    EEPROM.begin(512);

    display.setTextSize(4);                  // Set text size
    display.println("Clearing EEPROM...\n"); // Print message
    display.display();                       // Full refresh the display
    clearEEPROM();                           // Clear user EEPROM data
    delay(500);                              // Wait a little bit...

    display.println("Writing data to EEPROM...\n"); // Print message
    display.partialUpdate();                        // Use partial updates for refreshing the display
    writeEEPROM();                                  // Write some data to EEPROM
    delay(500);                                     // Wait a little bit...

    display.println("Reading data from EEPROM:\n"); // Print message
    display.partialUpdate();                        // Use partial updates for refreshing the display
    display.setTextSize(2);                         // Use smaller text so everything can fit on display
    printEEPROM();                                  // Read data from EEPROM and display it on screen
    delay(500);                                     // Wait a little bit...
}

void loop()
{
    // Empty...
}

// Function for clearing EEPROM data (it will NOT clear waveform data)
void clearEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i + EEPROM_START_ADDR,
                     0); // Start writing from address 76 (anything below that address number is waveform data!)
    }
    EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i + EEPROM_START_ADDR,
                     i); // Start reading from address 76 (anything below that address number is waveform data!)
    }
    EEPROM.commit();
}

// Function reads back previously written data and displays it on screen.
void printEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        display.print(EEPROM.read(i + EEPROM_START_ADDR), DEC);
        if (i != EEPROM_SIZE - 1)
            display.print(", ");
    }
    display.partialUpdate();
}
