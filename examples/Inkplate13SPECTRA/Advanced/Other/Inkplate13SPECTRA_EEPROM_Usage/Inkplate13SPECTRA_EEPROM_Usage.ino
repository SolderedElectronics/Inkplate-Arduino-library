/**
 **************************************************
 * @file        Inkplate13SPECTRA_EEPROM_Usage.ino
 * @brief       EEPROM read/write example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to use EEPROM with the Inkplate board. EEPROM is a
 *              permanent memory that holds data even if the power supply is
 *              disconnected, so you can use it to store any data you don't want
 *              to lose when restarting or powering down the device. The example
 *              demonstrates the basic EEPROM operations: clearing, writing and
 *              reading.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The sketch clears the user EEPROM area, writes sample data and reads it
 *    back.
 * 4) The written and read values are shown on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-01-22
 **************************************************/




// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_SIZE 128 // How much data to write to EEPROM in this example

Inkplate inkplate; // Create object on Inkplate library and set library to work in monochrome mode

void setup() {
  Serial.begin(115200);                     // Init serial monitor to display what's happening
  inkplate.begin();                         // Init library (you should call this function ONLY ONCE)
  inkplate.setTextColor(INKPLATE_BLACK);    // Set text color to black

  // Init EEPROM library with 128 of EEPROM size.
  EEPROM.begin(EEPROM_SIZE);

  inkplate.setTextSize(4);                // Set text size
  Serial.println("Cleaning EEPROM...");   // Print message on serial monitor
  clearEEPROM();                          // Clear user EEPROM data
  delay(500);                             // Wait a little bit...

    Serial.println("Writing data to EEPROM..."); // Print message on serial monitor
    writeEEPROM();                               // Write some data to EEPROM
    delay(500);                                  // Wait a little bit...

  inkplate.println("Reading data from EEPROM:\n");  // Print message on the screen
  inkplate.println("Reding data from EEPROM...");   // Print message on the Sserial Monitor
  inkplate.setTextSize(2);                          // Use smaller text so everything can fit on display
  printEEPROM();                                    // Read data from EEPROM and display it on screen
  delay(500);                                       // Wait a little bit...

}

void loop() {
  // Empty...
}

// Function for clearing EEPROM data
void clearEEPROM()
{
  for(int i=0;i<EEPROM_SIZE;i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
  for(int i=0;i<EEPROM_SIZE;i++){
    EEPROM.write(i,i);
  }
  EEPROM.commit();
}

// Function reads back previously written data and displays it on serial monitor and screen.
void printEEPROM()
{
  for(int i=0;i<EEPROM_SIZE;i++){
    inkplate.print(EEPROM.read(i), DEC);
    Serial.print(EEPROM.read(i), DEC);
    if(i != EEPROM_SIZE -1)
    {
      inkplate.print(", ");
      Serial.print(", ");
    }
  }
  inkplate.display();
}
