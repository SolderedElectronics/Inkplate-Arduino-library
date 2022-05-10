/*
    Inkplate_Blink example for e-radionica.com Inkplate 2
    For this example you will need USB cable, Inkplate 2, resistor
    and one LED (and some wires and breadboard to connect it).
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can control I/O pins.
    You should consider that only pins that are on header available
    for use with external devices.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    10 May 2022 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"
#include "Inter6pt7b.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize Inkplate
    display.begin();

    display.setFont(&Inter6pt7b);

    display.setTextSize(1);      // Set text size
    display.setCursor(10, 20);     // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Blink example");
    display.setCursor(10, 35);     // Set cursor position
    display.println("Connect LED to IO14 and LED will blink once every two seconds.");

    // Display to screen
    display.display();

    // Set pin 14 to be output pin
    pinMode(14, OUTPUT);
}

void loop()
{
    digitalWrite(14, HIGH); // Set pin 14 to HIGH state
    delay(1000); // Wait a bit
    digitalWrite(14, LOW); // Set pin 14 to LOW state
    delay(1000); // Wait a bit
}
