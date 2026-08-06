/**
 **************************************************
 * @file        Inkplate6PLUS_Hello_World.ino
 * @brief       Basic "Hello World" example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates the simplest way to use the Inkplate library by
 *              displaying the text "Hello World!" on the e-paper screen.
 *              The example initializes the display, writes text to the
 *              framebuffer using Adafruit GFX-compatible functions, and then
 *              refreshes the screen to show the content.
 *
 *              clearDisplay() clears only the internal framebuffer, so
 *              display.display() must be called to refresh the physical e-paper
 *              panel. Inkplate text functions are fully compatible with Adafruit
 *              GFX. Expected output is the text "Hello World!" near the top-left
 *              corner of the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6PLUS"
 *    from Tools -> Board.
 * 2) Upload the sketch to your Inkplate device.
 * 3) After initialization, the text "Hello World!" appears on the display.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Adafruit GFX reference:
 *              https://learn.adafruit.com/adafruit-gfx-graphics-library
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************/



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
