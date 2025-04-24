/*
   Inkplate6PLUS_Hello_World example for Soldered Inkplate 6PLUS

   For this example, you only need a USB cable and your Inkplate 6PLUS.
   Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from the Tools -> Board menu in Arduino IDE.
   Don't see the "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow this tutorial to add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example demonstrates the most basic usage: displaying "Hello World!" on the screen.
   It uses the Inkplate library's built-in text drawing functions, fully compatible with Adafruit GFX.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Need support? Visit our forums: https://forum.soldered.com/
   24 April 2025 by Soldered
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display(INKPLATE_1BIT); // Create an Inkplate object for Inkplate 6PLUS

void setup() {
    display.begin();             // Initialize the display hardware
    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);   // Set the text position to (10, 10) pixels
    display.setTextSize(4);      // Set text size to 4 (default is 1)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();           // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
