/*
    Inkplate7_Text_With_Shadow example for Soldered Inkplate 7
    For this example you will need only USB-C cable and Inkplate 7.
    Select "Soldered Inkplate7" from Tools -> Board menu.
    Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can draw some simple graphics using
    Adafruit GFX functions. It will draw red text with black shadow.
    Inkplate library is 100% compatible with Adafruit GFX lib!
    Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    25 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Init library (you should call this function ONLY ONCE)
    display.begin();

    display.setTextSize(5);                                 // Set text size to be 5 times bigger than original (5x7 px)
    display.setTextColor(INKPLATE7_BLACK, INKPLATE7_WHITE); // Set the text color to black, and the background to white
    display.setCursor(70, 80);                              // Set cursor position

    // Draw black text
    display.print("Inkplate 7"); // Print text in the frame buffer

    // Draw red text
    display.setTextColor(INKPLATE7_RED);
    display.setCursor(70, 130);
    display.println("Inkplate 7");

    // Draw text with shadow, specify color for text and background
    // (x coordinate, y coordinate, string to write, text color, shadow color)
    display.drawTextWithShadow(70, 180, "Inkplate 7", INKPLATE7_RED, INKPLATE7_BLACK);

    // Draw a text with shadow with swapped colors
    display.setTextSize(3);
    display.drawTextWithShadow(330, 280, "By Soldered", INKPLATE7_BLACK, INKPLATE7_RED);

    // Other basic drawing functions supported
    display.drawLine(70, 250, 300, 350, INKPLATE7_RED);
    display.drawCircle(500, 150, 75, INKPLATE7_BLACK);

    // Display to screen
    display.display();

    // Go to deep sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Empty...
}
