/*
    Inkplate_Blink example for soldered.com Inkplate 2
    For this example you will need USB cable, Inkplate 2, 330Ohm resistor
    and one LED (and some wires and breadboard to connect it).
    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can control I/O pins.
    You should consider that only pins that are on header available
    for use with external devices.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    10 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"

// LED is connected to ESP32 GPIO 14
#define PIN_LED 14

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.setTextSize(1);    // Set text size
    display.setCursor(10, 20); // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Blink example");
    display.setCursor(10, 35); // Set cursor position
    display.println("Connect LED to ESP32 GPIO14 and LED will blink once every two seconds.");

    // Display to screen
    display.display();

    // Set LED GPIO to be output pin
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    digitalWrite(PIN_LED, HIGH); // Turn on LED
    delay(1000);                 // Wait a bit
    digitalWrite(PIN_LED, LOW);  // Turn off LED
    delay(1000);                 // Wait a bit
}
