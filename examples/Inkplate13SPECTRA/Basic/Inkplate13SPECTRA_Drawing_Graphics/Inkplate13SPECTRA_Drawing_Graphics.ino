/*
   Inkplate13SPECTRA_Drawing_Graphics  example for Soldered Inkplate 13SPECTRA
   For this example you will need a micro USB cable and Inkplate 13SPECTRA.
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

   /////////////////////////// INSERT BOARD DEF LINK HERE

   This example will show you how to draw shapes, lines and text using the Adafruit GFX
   functions built into the Inkplate library.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   23 January 2026 by Soldered Electronics
*/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.clearDisplay();
    display.fillScreen(INKPLATE_WHITE);

    // Rectangles
    display.drawRect(40, 40, 520, 240, INKPLATE_BLACK);
    display.fillRect(60, 60, 200, 80, INKPLATE_YELLOW);
    display.drawRect(60, 60, 200, 80, INKPLATE_BLACK);
    display.fillRoundRect(290, 60, 240, 80, 16, INKPLATE_RED);
    display.drawRoundRect(290, 60, 240, 80, 16, INKPLATE_BLACK);

    // Circles
    display.drawCircle(160, 220, 60, INKPLATE_BLUE - 1);
    display.fillCircle(380, 220, 60, INKPLATE_GREEN - 1);
    display.drawCircle(380, 220, 60, INKPLATE_BLACK);

    // Triangle
    display.fillTriangle(520, 320, 640, 160, 760, 320, INKPLATE_YELLOW);
    display.drawTriangle(520, 320, 640, 160, 760, 320, INKPLATE_BLACK);

    // Lines
    display.drawLine(40, 360, 760, 520, INKPLATE_BLACK);
    display.drawLine(40, 390, 760, 550, INKPLATE_RED);
    display.drawLine(40, 420, 760, 580, INKPLATE_YELLOW);
    display.drawLine(40, 450, 760, 610, INKPLATE_BLUE - 1);
    display.drawLine(40, 480, 760, 640, INKPLATE_GREEN - 1);

    // Simple text
    display.setTextSize(3);
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(40, 760);
    display.print("Inkplate 13SPECTRA");

    display.setTextSize(2);
    display.setCursor(40, 800);
    display.print("Adafruit_GFX shapes & colors");

    // Update e-paper
    display.display();
}

void loop()
{
    // Loop forever
}
