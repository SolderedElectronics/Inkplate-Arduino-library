/*
    Inkplate4_Text_With_Shadow example for Soldered Inkplate 4
    For this example you will need only USB cable and Inkplate 4.
    Select "Soldered Inkplate4" from Tools -> Board menu.
    Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can draw some simple graphics using
    Adafruit GFX functions. It will draw red text with black shadow.
    Inkplate library is 100% compatible with Adafruit GFX lib!
    Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    31 July 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include "Inkplate.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate library
    display.begin();

    display.setTextSize(4);                // Set text size
    display.setTextColor(INKPLATE4_BLACK); // Set text color
    display.setCursor(80, 60);               // Set cursor position

    // Draw black text
    display.print("Inkplate 4"); // Print text

    // Draw red text
    display.setTextColor(INKPLATE4_RED);
    display.setCursor(80, 105);
    display.print("Inkplate 4");

    // Draw text with shadow, specify color for text and background
    // (x coordinate, y coordinate, string to write, text color, shadow color)
    display.drawTextWithShadow(80, 150, "Inkplate 4", INKPLATE4_RED, INKPLATE4_BLACK);

    display.setTextSize(2);                // Set text size
    display.drawTextWithShadow(105, 195, "By soldered.com", INKPLATE4_BLACK, INKPLATE4_RED);

    // Display to screen
    display.display();

    // Go to deep sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Empty...
}
