/*
   Inkplate_Clean example for Soldered Inkplate 6PLUS
   For this example you will need only USB cable and Inkplate 6PLUS.
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will try to remove heavy burn-in visible on the panel.
   Set number of refresh / clear cycles and upload the program.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   12 January 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select Inkplate 6PLUS or Inkplate 6PLUS V2 in the boards menu."
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
        display.clean(1, 15);
        display.clean(2, 1);
        display.clean(0, 5);
        display.clean(2, 1);
        display.clean(1, 15);
        display.clean(2, 1);
        display.clean(0, 5);
        display.clean(2, 1);

        delay(CYCLES_DELAY);
        cycles--;
    }

    // Print text when clearing is done.
    display.setTextSize(4);
    display.setCursor(100, 100);
    display.print("Clearing done.");
    display.display();
}

void loop()
{
    // Empty...
}