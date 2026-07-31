/**
 **************************************************
 * @file        Inkplate5V2_Hello_World.ino
 * @brief       Basic "Hello World" example for Soldered Inkplate 5v2.
 *
 * @details     Demonstrates the most basic usage of the Inkplate 5v2 by
 *              initializing the display and printing "Hello World!" on the
 *              e-paper screen. The example uses built-in text rendering
 *              functions fully compatible with the Adafruit GFX library and
 *              runs in 1-bit (black & white) display mode.
 *
 *              display.clearDisplay() clears only the internal framebuffer,
 *              so display.display() must be called to update the physical
 *              e-paper panel. Expected output is the text "Hello World!"
 *              shown on the Inkplate screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5v2.
 * 3) After initialization, "Hello World!" appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Image converter tool:
 *              https://tools.soldered.com/tools/image-converter/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5V2 in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display(INKPLATE_1BIT); // Create an Inkplate object for Inkplate5V2

void setup() {
    display.begin();            // Initialize the display hardware
    display.clearDisplay();     // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);  // Set the text position to (10, 10) pixels
    display.setTextSize(8);     // Set text size to 8 (default is 1)
    display.setTextColor(BLACK); // Set text color to black
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();          // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
