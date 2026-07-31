/*
   Inkplate13SPECTRA_Full_Screen_Colors example for Soldered Inkplate 6COLOR
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:
   https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

   Simple Inkplate example showing all colors of the Inkplate.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   Looking to get support? Write on our community forum: https://community.soldered.com/
   23 January 2026 by Soldered
*/

#include "Inkplate.h"

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate Inkplate 13SPECTRA in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.clearDisplay();
    // Draw a full screen of all colors
    display.fillRect(0, 0, 1600 / 6 + 2, 1200, INKPLATE_BLACK);
    display.fillRect(1 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_WHITE);
    display.fillRect(2 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_YELLOW);
    display.fillRect(3 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_RED);
    display.fillRect(4 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_BLUE-1);
    display.fillRect(5 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_GREEN-1);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}
