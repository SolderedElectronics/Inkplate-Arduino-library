/*
   Inkplate_Clean example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will try to remove heavy burn-in visible on the panel.
   Set number of refresh / clear cycles and upload the program.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   12 January 2022 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 5 in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display; // Create object on Inkplate library and set library to work in monochorme mode

// Nubmer of clear cycles.
#define CLEAR_CYCLES 5

// Delay between clear cycles (in milliseconds)
#define CYCLES_DELAY 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

    int cycles = CLEAR_CYCLES;

    // Clean it by writing clear sequence to the panel.
    while (cycles)
    {
        cycles--;
        display.display();
        delay(CYCLES_DELAY);
    }

    // Print text when clearing is done.
    display.setTextSize(4);
    display.setCursor(10, 15);
    display.setTextColor(RED,WHITE);
    display.print("Clearing done.");
    display.display();
}

void loop()
{
    // Empty...
}
