/*
   Inkplate6COLOR_Hello_World example for Soldered Inkplate 6COLOR

   Select "Soldered Inkplate 6COLOR" from the Tools -> Board menu in Arduino IDE.
   Don't see the "Soldered Inkplate 6COLOR" option? Follow this tutorial to add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example demonstrates the most basic usage: displaying "Hello World!" on the screen using the Inkplate 6COLOR library.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Need support? Visit our forums: https://forum.soldered.com/
   24 April 2025 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display; // Create an Inkplate object for Inkplate 6COLOR

void setup() {
    display.begin();             // Initialize the display hardware
    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);   // Set the text position to (10, 10) pixels
    display.setTextSize(4);      // Set text size to 4 (default is 1)
    display.setTextColor(INKPLATE_BLACK); // Set text color to black
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();           // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}

