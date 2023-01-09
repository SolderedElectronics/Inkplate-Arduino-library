/*
    Inkplate_Basic_Text_With_Shadow example for Soldered Inkplate 2
    For this example you will need only USB cable and Inkplate 2.
    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can draw some simple graphics using
    Adafruit GFX functions. It will draw red text with black shadow.
    Inkplate library is 100% compatible with Adafruit GFX lib!
    Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    15 Feb 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate library
    display.begin();

    display.setTextSize(2);                // Set text size
    display.setTextColor(INKPLATE2_BLACK); // Set text color
    display.setCursor(0, 0);               // Set cursor position

    // Draw black text
    display.print("Inkplate 2"); // Print text

    // Draw red text
    display.setTextColor(INKPLATE2_RED);
    display.setCursor(0, 20);
    display.print("Inkplate 2");

    // Draw text with shadow, specify color for text and background
    // (x coordinate, y coordinate, string to write, text color, shadow color)
    display.drawTextWithShadow(0, 40, "Inkplate 2", INKPLATE2_RED, INKPLATE2_BLACK);

    display.setTextSize(1);
    display.drawTextWithShadow(110, 80, "By soldered.com", INKPLATE2_BLACK, INKPLATE2_RED);

    // Other basic drawing functions supported
    display.drawLine(10, 80, 100, 100, INKPLATE2_RED);

    // Display to screen
    display.display();

    // Go to deep sleep
    display.setPanelDeepSleep(0); // Zero will put panel into sleep mode, one will wake it up
    esp_deep_sleep_start();       // This will put ESP32 into deep sleep mode
}

void loop()
{
    // Empty...
}
