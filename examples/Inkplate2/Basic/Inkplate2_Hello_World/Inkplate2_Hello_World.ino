/*
    Inkplate2_Hello_World example for Soldered Inkplate 2

    For this example, you only need a USB cable and your Inkplate 2.
    Select "Soldered Inkplate2" from the Tools -> Board menu in Arduino IDE.
    Don't see the "Soldered Inkplate2" option? Follow this tutorial to add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example demonstrates the most basic usage: displaying "Hello World!" on the screen.
    It uses the Inkplate library's built-in text drawing function for Inkplate2.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Need support? Visit our forums: https://forum.soldered.com/
    24 April 2025 by Soldered
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display;     // Create an Inkplate object in monochrome mode

void setup() {
    display.begin();           // Initialize the display hardware
    display.clearDisplay();    // Clear the software frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10); // Set the text position to (10, 10) pixels
    display.setTextSize(2);    // Set text size to 2 (default is 1)
    display.setTextColor(1);   // Set text color to black (1 = black, 0 = white)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();         // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
