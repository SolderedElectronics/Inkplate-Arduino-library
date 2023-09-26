/*
   Inkplate4TEMPERA_Burn_In_Clean example for Soldered Inkplate 4TEMPERA
   For this example you will need only USB-C cable and Inkplate 4TEMPERA.
   Select "Soldered Inkplate 4TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will try to remove heavy burn-in visible on the panel.
   Set number of refresh / clear cycles and upload the program.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   24 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4TEMPERA in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochorme mode

// Nubmer of clear cycles.
#define CLEAR_CYCLES 20

// Delay between clear cycles (in milliseconds)
#define CYCLES_DELAY 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

    display.einkOn(); // Power up epaper power supply

    int cycles = CLEAR_CYCLES;

    // Clean it by writing clear sequence to the panel.
    while (cycles)
    {
        display.clean(1, 21);
        display.clean(2, 1);
        display.clean(0, 12);
        display.clean(2, 1);
        display.clean(1, 21);
        display.clean(2, 1);
        display.clean(0, 12);
        display.clean(2, 1);

        delay(CYCLES_DELAY);
        cycles--;
    }

    // Print text when clearing is done.
    display.setTextSize(4);
    display.setCursor(125, 270);
    display.print("Clearing done.");
    display.display();
}

void loop()
{
    // Empty...
}
