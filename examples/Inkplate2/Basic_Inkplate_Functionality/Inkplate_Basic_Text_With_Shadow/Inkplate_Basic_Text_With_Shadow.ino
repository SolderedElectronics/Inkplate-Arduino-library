/*
   Inkplate_Basic example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can draw some simple graphics using
   Adafruit GFX functions. Yes, Inkplate library is 100% compatible with GFX lib!
   Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 Feb 2022 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize Inkplate
    display.begin();

    display.setTextSize(2);      // Set text size
    display.setTextColor(BLACK); // Set text color
    display.setCursor(0, 0);     // Set cursor position

    display.print("Test"); // Print text

    display.setTextColor(RED);
    display.setCursor(0, 20);
    display.print("Test");

    // Draw text with shadow, specify color for text and background
    // (x coordinate, y coordinate, string to write, text color, shadow color)
    display.drawTextWithShadow(0, 40, "Test", RED, BLACK);

    display.setTextSize(1);
    display.drawTextWithShadow(110, 80, "By soldered.com", RED, BLACK);

    // Other basic drawing functions supported
    display.drawLine(10, 80, 100, 100, RED);

    // Display to screen
    display.display();

    // Go to deep sleep
    display.setPanelDeepSleep(0);
    esp_deep_sleep_start();
}

void loop()
{
    // Empty...
}
