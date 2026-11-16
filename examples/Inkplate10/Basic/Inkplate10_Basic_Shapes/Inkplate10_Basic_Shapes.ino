/**
 **************************************************
 * @file        Inkplate10_Basic_Shapes.ino
 * @brief       Basic shapes example for Soldered Inkplate 10.
 *
 * @details     Draws the ten basic shapes of the Adafruit GFX
 *              compatible drawing functions on the Inkplate 10, in
 *              two rows: a rectangle, a circle, a rounded rectangle,
 *              a triangle and a line, then the filled versions and a
 *              few single pixels.
 *
 *              Expected output is two rows of shapes on the Inkplate
 *              screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) After initialization, the shapes appear on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select Soldered Inkplate 10 in the boards menu."
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
  // each one is on the line y = 250.

  // drawRect(x, y, width, height, colour): x, y is the top left corner.
  display.drawRect(40, 170, 160, 160, BLACK);

  // drawCircle(x, y, radius, colour): x, y is the centre.
  display.drawCircle(360, 250, 80, BLACK);

  // drawRoundRect(x, y, width, height, radius, colour): a rectangle with round corners.
  display.drawRoundRect(520, 170, 160, 160, 30, BLACK);

  // drawTriangle(x1, y1, x2, y2, x3, y3, colour): joins three points.
  display.drawTriangle(840, 170, 760, 330, 920, 330, BLACK);

  // drawLine(x1, y1, x2, y2, colour): joins two points.
  display.drawLine(1000, 330, 1160, 170, BLACK);

  // Row 2, the solid shapes. They have the same sizes as the outlines above,
  // and the middle of each one is on the line y = 580.
  display.fillRect(40, 500, 160, 160, BLACK);
  display.fillCircle(360, 580, 80, BLACK);
  display.fillRoundRect(520, 500, 160, 160, 30, BLACK);
  display.fillTriangle(840, 500, 760, 660, 920, 660, BLACK);

  // drawPixel(x, y, colour) sets a single pixel, which is tiny, so five of them
  // make a small plus.
  display.drawPixel(1080, 580, BLACK);
  display.drawPixel(1072, 580, BLACK);
  display.drawPixel(1088, 580, BLACK);
  display.drawPixel(1080, 572, BLACK);
  display.drawPixel(1080, 588, BLACK);

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
