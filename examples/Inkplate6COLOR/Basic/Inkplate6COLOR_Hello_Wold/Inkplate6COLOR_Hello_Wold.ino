/**
 **************************************************
 * @file        Inkplate6COLOR_Hello_Wold.ino
 * @brief       Displays a simple "Hello World!" message on Inkplate 6COLOR.
 *
 * @details     This example demonstrates the most basic workflow for using the
 *              Inkplate 6COLOR library. The sketch initializes the display,
 *              clears the framebuffer, prints a short text string, and then
 *              refreshes the e-paper panel so the content becomes visible.
 *
 *              It introduces the fundamental drawing sequence used in most
 *              Inkplate sketches:
 *
 *              1) Initialize the display with begin()
 *              2) Draw content into the framebuffer
 *              3) Trigger a display refresh with display()
 *
 *              This example is intended as a minimal starting point for
 *              beginners who want to verify that their board and software
 *              environment are working correctly. clearDisplay() only clears the
 *              internal framebuffer; the physical display updates only when
 *              display() is called. The sketch performs a single full refresh
 *              and does not update again - colour e-paper refreshes are slower
 *              than monochrome displays and should not be triggered
 *              unnecessarily in real applications.
 *
 *              Expected output: the message "Hello World!" printed near the
 *              top-left corner of the screen using a large font.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     not used in this example
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to the board.
 * 3) After initialization, the text "Hello World!" appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/

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

