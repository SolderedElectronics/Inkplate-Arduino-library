/*
   Inkplate7SPECTRA_Full_Screen_Colors example for Soldered Inkplate 7SPECTRA
   Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
   https://docs.soldered.com/inkplate/7spectra/quick-start-guide/

   Simple Inkplate example showing all colors of the Inkplate.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   Looking to get support? Write on our forums: https://forum.soldered.com/
   17 July 2026 by Soldered
*/

#include "Inkplate.h"

#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.clearDisplay();
    // Draw a full screen of all colors
    display.fillRect(0, 0, 800 / 6 + 2, 480, INKPLATE_BLACK);
    display.fillRect(1 * 800 / 6, 0, 800 / 6 + 2, 480, INKPLATE_WHITE);
    display.fillRect(2 * 800 / 6, 0, 800 / 6 + 2, 480, INKPLATE_YELLOW);
    display.fillRect(3 * 800 / 6, 0, 800 / 6 + 2, 480, INKPLATE_RED);
    display.fillRect(4 * 800 / 6, 0, 800 / 6 + 2, 480, INKPLATE_BLUE);
    display.fillRect(5 * 800 / 6, 0, 800 / 6 + 2, 480, INKPLATE_GREEN);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}
