/*
   Inkplate13SPECTRA_Custom_Font  example for Soldered Inkplate 13SPECTRA
   For this example you will need a micro USB cable and Inkplate 13SPECTRA.
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

   /////////////////////////// INSERT BOARD DEF LINK HERE

   This example will show you how to print text using a custom font instead of the default one.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   23 January 2026 by Soldered Electronics
*/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"
#include "FreeMono9pt7b.h"

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.display();
    display.setFont(&FreeMono9pt7b);
    display.setCursor(100, 100);
    display.setTextColor(INKPLATE_BLACK);
    display.setTextSize(3);
    display.print("Hello World!");
    display.display();
}

void loop()
{
}
