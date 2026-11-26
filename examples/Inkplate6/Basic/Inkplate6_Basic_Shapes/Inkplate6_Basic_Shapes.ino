/**
 **************************************************
 * @file        Inkplate6_Basic_Shapes.ino
 * @brief       Basic shapes example for Soldered Inkplate 6.
 *
 * @details     Draw the ten basic shapes on the Inkplate 6,
 *              in black and white. Connection: none, the
 *              screen is on the board. Power and upload over
 *              USB-C.
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
 * 3) The ten shapes appear on the display.
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

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  // Start the display. Call this once, at the start.
  display.begin();

  // Empty the frame buffer
  display.clearDisplay();

  // Row 1, the outlines. Every shape sits in its own column, and the middle of
  // each one is on the line y = 170.

  // drawRect(x, y, width, height, colour): x, y is the top left corner.
  display.drawRect(30, 120, 100, 100, BLACK);

  // drawCircle(x, y, radius, colour): x, y is the centre.
  display.drawCircle(230, 170, 50, BLACK);

  // drawRoundRect(x, y, width, height, radius, colour): a rectangle with round corners.
  display.drawRoundRect(330, 120, 100, 100, 20, BLACK);

  // drawTriangle(x1, y1, x2, y2, x3, y3, colour): joins three points.
  display.drawTriangle(530, 120, 480, 220, 580, 220, BLACK);

  // drawLine(x1, y1, x2, y2, colour): joins two points.
  display.drawLine(630, 220, 730, 120, BLACK);

  // Row 2, the solid shapes. They have the same sizes as the outlines above,
  // and the middle of each one is on the line y = 420.
  display.fillRect(30, 370, 100, 100, BLACK);
  display.fillCircle(230, 420, 50, BLACK);
  display.fillRoundRect(330, 370, 100, 100, 20, BLACK);
  display.fillTriangle(530, 370, 480, 470, 580, 470, BLACK);

  // drawPixel(x, y, colour) sets a single pixel, which is tiny, so five of them
  // make a small plus.
  display.drawPixel(680, 420, BLACK);
  display.drawPixel(672, 420, BLACK);
  display.drawPixel(688, 420, BLACK);
  display.drawPixel(680, 412, BLACK);
  display.drawPixel(680, 428, BLACK);

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
