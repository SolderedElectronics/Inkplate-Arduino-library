/*
   InkplatePLUS2_EasyC example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2 and Digital Hall Effect Sensor with easyC
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu. Don't have
   "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read if a magnet is detected near the Hall Sensor.

   IMPORTANT: To run this example, install the Hall Effect Library:
   https://github.com/SolderedElectronics/Soldered-Hall-Effect-Sensor-Arduino-Library

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   13 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Soldered InkplatePLUS2 in the boards menu."
#endif

#include "Hall-Effect-Digital-easyC-SOLDERED.h" // Include the Hall Effect sensor library
#include "Inkplate.h"                           // Include Inkplate library to the sketch

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

// Create sensor object on default address
HallEffect_Digital_EasyC hall;

int numPartialUpdates = 0; // Variable to keep track of the number of partial updates

bool lastReadState = false; // To remember the last read state of magnet/no magnet detected

void setup()
{
    Serial.begin(112500); // So we can see the printed output

    display.begin();           // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();    // Clear the display
    display.setCursor(30, 30); // Set the cursor a bit further from the corner so we can see the text
    display.setTextSize(2);    // Scale text to be 2 times bigger then original (5x7 px)
    display.print("Place a magnet near the Hall sensor!");
    display.display(); // Show the text on the display

    hall.begin(); // Initialize Hall Effect sensor via EasyC (required!)
}

void loop()
{
    // Read sensor
    bool hallReading = hall.getReading();

    // In case there was a change, update the display and print data to Serial
    if (hallReading != lastReadState)
    {
        lastReadState = hallReading; // Remember the state

        // Print the reading on Inkplate and on Serial
        if (hallReading)
        {
            display.clearDisplay();    // Clear the display
            display.setCursor(30, 30); // Set the cursor a bit further from the corner so we can see the text
            display.setTextSize(2);    // Scale text to be 2 times bigger then original (5x7 px)
            display.print("Magnet detected!");

            // Do partial or full update depending on the counter
            if (numPartialUpdates < 10)
            {
                display.partialUpdate(); // Do partial update
                numPartialUpdates++;     // Increment the counter
            }
            else
            {
                numPartialUpdates = 0; // Reset the counter
                display.display();     // Show the text on the display with full update
            }

            // Also, print to Serial
            Serial.println("Magnet detected!\n");
        }
        else
        {
            display.clearDisplay();    // Clear the display
            display.setCursor(30, 30); // Set the cursor a bit further from the corner so we can see the text
            display.setTextSize(2);    // Scale text to be 2 times bigger then original (5x7 px)
            display.print("No magnet detected!");

            // Do partial or full update depending on the counter
            if (numPartialUpdates < 10)
            {
                display.partialUpdate(); // Do partial update
                numPartialUpdates++;     // Increment the counter
            }
            else
            {
                numPartialUpdates = 0; // Reset the counter
                display.display();     // Show the text on the display with full update
            }

            // Also, print to Serial
            Serial.println("No magnet detected.\n");
        }
    }

    // Wait a bit until the next measurement
    delay(500);
}