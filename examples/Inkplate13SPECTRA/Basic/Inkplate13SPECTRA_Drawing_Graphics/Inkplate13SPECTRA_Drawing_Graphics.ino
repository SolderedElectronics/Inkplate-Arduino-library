/**
 **************************************************
 * @file        Inkplate13SPECTRA_Drawing_Graphics.ino
 * @brief       Draw the ten basic shapes on the Inkplate 13, in colour.
 *
 * @details     Connection: none, the screen is on the board. Power and upload
 *              over USB-C.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The sketch runs as described above.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

// Pull in the Inkplate library
#include "Inkplate.h"

// The display object
Inkplate display;

// setup() runs once, right after the board powers on or resets
void setup() {
  // Start the display. Call this once, at the start.
  display.begin();

  // Empty the frame buffer
  display.clearDisplay();

  // Row 1, the outlines. Every shape sits in its own column, and the middle of
  // each one is on the line y = 330.

  // drawRect(x, y, width, height, colour): x, y is the top left corner.
  display.drawRect(60, 230, 200, 200, INKPLATE_BLACK);

  // drawCircle(x, y, radius, colour): x, y is the centre.
  display.drawCircle(480, 330, 100, INKPLATE_RED);

  // drawRoundRect(x, y, width, height, radius, colour): a rectangle with round corners.
  display.drawRoundRect(700, 230, 200, 200, 40, INKPLATE_BLUE);

  // drawTriangle(x1, y1, x2, y2, x3, y3, colour): joins three points.
  display.drawTriangle(1120, 230, 1020, 430, 1220, 430, INKPLATE_GREEN);

  // drawLine(x1, y1, x2, y2, colour): joins two points.
  display.drawLine(1340, 430, 1540, 230, INKPLATE_BLACK);

  // Row 2, the solid shapes. They have the same sizes as the outlines above,
  // and the middle of each one is on the line y = 800.
  display.fillRect(60, 700, 200, 200, INKPLATE_YELLOW);
  display.fillCircle(480, 800, 100, INKPLATE_RED);
  display.fillRoundRect(700, 700, 200, 200, 40, INKPLATE_BLUE);
  display.fillTriangle(1120, 700, 1020, 900, 1220, 900, INKPLATE_GREEN);

  // drawPixel(x, y, colour) sets a single pixel, which is tiny, so five of them
  // make a small plus.
  display.drawPixel(1440, 800, INKPLATE_BLACK);
  display.drawPixel(1432, 800, INKPLATE_BLACK);
  display.drawPixel(1448, 800, INKPLATE_BLACK);
  display.drawPixel(1440, 792, INKPLATE_BLACK);
  display.drawPixel(1440, 808, INKPLATE_BLACK);

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
