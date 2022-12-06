/*
   Inkplate_Full_Screen_Colors sketch for Soldered Inkplate 6COLOR
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   Simple Inkplate example showing all colors of the Inkplate.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   27 September 2021 by Soldered
*/

#include "Inkplate.h"

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    // Draw a full screen of all colors
    display.fillRect(0, 0, 600 / 7 + 2, 448, INKPLATE_BLACK);
    display.fillRect(1 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_WHITE);
    display.fillRect(2 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_GREEN);
    display.fillRect(3 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_BLUE);
    display.fillRect(4 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_RED);
    display.fillRect(5 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_YELLOW);
    display.fillRect(6 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_ORANGE);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}