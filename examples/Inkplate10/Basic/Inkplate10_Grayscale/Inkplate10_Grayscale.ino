/**
 **************************************************
 * @file        Inkplate10_Grayscale.ino
 * @brief       Grayscale (3-bit) bars example for Soldered Inkplate
 *              10.
 *
 * @details     Shows the eight gray levels of the Inkplate 10 as
 *              vertical bars across the screen. In 3-bit grayscale
 *              mode the display supports 8 shades (0-7), where 0 is
 *              black, 7 is white and values in between are gray
 *              levels.
 *
 *              Grayscale mode needs a full refresh for every update,
 *              so partial update is not available in this mode.
 *              Expected output is eight vertical bars going from
 *              black to white.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) After initialization, the eight gray bars appear on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Adafruit GFX reference:
 *              https://learn.adafruit.com/adafruit-gfx-graphics-library
 *
 * @author      Josip Šimun Kuči @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the Inkplate library
#include "Inkplate.h"

// Gray mode: eight levels, 0 (black) to 7 (white)
Inkplate display(INKPLATE_3BIT);

// How wide each bar is, in pixels. Eight bars of 150 pixels fill the 1200 pixel screen.
const int barWidth = 150;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();

  // One bar for each gray level
  for (int level = 0; level < 8; level++) {
    // A solid rectangle: left edge, top edge, width, height, then the gray level
    display.fillRect(level * barWidth, 0, barWidth, 825, level);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
