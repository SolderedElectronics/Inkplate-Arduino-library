/**
 **************************************************
 * @file        Inkplate6_Display_Modes.ino
 * @brief       Grayscale (3-bit) bars example for Soldered
 *              Inkplate 6.
 *
 * @details     Show the eight gray levels of the Inkplate 6
 *              as vertical bars across the screen.
 *              Connection: none, the screen is on the board.
 *              Power and upload over USB-C.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6.
 * 3) The eight gray bars appear on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

// Pull in the Inkplate library
#include "Inkplate.h"

// Gray mode: eight levels, 0 (black) to 7 (white)
Inkplate display(INKPLATE_3BIT);

// How wide each bar is, in pixels. eight bars of 100 pixels fill the 800 pixel screen.
const int barWidth = 100;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();

  // One bar for each gray level
  for (int level = 0; level < 8; level++) {
    // A solid rectangle: left edge, top edge, width, height, then the gray level
    display.fillRect(level * barWidth, 0, barWidth, 600, level);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
