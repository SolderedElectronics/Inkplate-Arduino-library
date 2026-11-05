/**
 **************************************************
 * @file        Inkplate10_Coordinate_Grid.ino
 * @brief       Dotted coordinate grid example for Soldered Inkplate
 *              10.
 *
 * @details     Draws a dotted grid with numbers on the Inkplate 10 so
 *              you can see how the x and y coordinates map to pixels
 *              on the screen. The origin (0, 0) is the top left
 *              corner, x grows to the right and y grows downward.
 *
 *              The text uses the SourceSans3_24.h font file placed
 *              next to the sketch. Expected output is a grid of dots
 *              with their coordinates written next to them.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) After initialization, the coordinate grid appears on the
 *    display.
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

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);

  // A vertical dotted line every 100 pixels. drawPixel(x, y, colour) sets one pixel, and
  // a pixel every 8 pixels down the screen makes a dotted line.
  for (int x = 100; x < 1200; x += 100) {
    for (int y = 0; y < 825; y += 8) {
      display.drawPixel(x, y, BLACK);
    }
  }

  // The same for the horizontal lines
  for (int y = 100; y < 825; y += 100) {
    for (int x = 0; x < 1200; x += 8) {
      display.drawPixel(x, y, BLACK);
    }
  }

  // The x numbers along the top edge, one every 100 pixels, and the y numbers down the
  // left edge. Each number starts a few pixels right of and below the position it names.
  for (int x = 0; x < 1200; x += 100) {
    display.setCursor(x + 4, 4);
    display.print(x);
  }
  for (int y = 100; y < 825; y += 100) {
    display.setCursor(4, y + 4);
    display.print(y);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
