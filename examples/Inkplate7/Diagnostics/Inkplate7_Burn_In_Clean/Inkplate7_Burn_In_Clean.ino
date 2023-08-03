/*
   Inkplate7_Burn_In_Clean example for Soldered Inkplate 7
   For this example you will need only USB cable and Inkplate 7.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will try to remove heavy burn-in visible on the panel.
   Set number of refresh / clear cycles and upload the program.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   26 Apr 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
Inkplate display;     // Create object on Inkplate library

// Nubmer of clear cycles.
#define CLEAR_CYCLES 5

// Delay between clear cycles (in milliseconds)
#define CYCLES_DELAY 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

    int cycles = CLEAR_CYCLES; // Set the number of clear cycles

    // Clean it by writing clear sequence to the panel.
    while (cycles)
    {
        cycles--;
        display.display();
        delay(CYCLES_DELAY);
    }

    // Print text when clearing is done.
    display.setTextSize(4);
    display.setCursor(172, 169);
    display.setTextColor(INKPLATE7_RED, INKPLATE7_WHITE);
    display.print("Clearing done.");
    display.display();
}

void loop()
{
    // Empty...
}
