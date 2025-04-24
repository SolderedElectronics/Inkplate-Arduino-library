/*
   Inkplate10_Hello_World example for Soldered Inkplate 10

   For this example, you only need a USB cable and your Inkplate 10.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from the Tools -> Board menu in Arduino IDE.
   Don't see the "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow this tutorial to add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example demonstrates the most basic usage: displaying "Hello World!" on the screen.
   It uses the Inkplate library's built-in text drawing functions, fully compatible with Adafruit GFX.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Need support? Visit our forums: https://forum.soldered.com/
   24 April 2025 by Soldered

   In order to convert your images into a format compatible with Inkplate,
   use the Soldered Image Converter available at:
   https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display(INKPLATE_1BIT); // Create an Inkplate object for Inkplate6 FLICK

void setup() {
    display.begin();             // Initialize the display hardware
    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);   // Set the text position to (10, 10) pixels
    display.setTextSize(6);      // Set text size to 6 (default is 1)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();           // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
