/**
 **************************************************
 * @file        Inkplate6_Rotation.ino
 * @brief       Screen rotation example for Soldered Inkplate
 *              6.
 *
 * @details     Draw the grid of T1 on the Inkplate 6 with the
 *              screen turned upright. Connection: none, the
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
 * 3) The grid appears with the screen turned upright.
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

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();

  // Turn the screen upright. Do this before anything is drawn.
  display.setRotation(1);

  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);

  // A vertical dotted line every 100 pixels. width() and height() give the size of the
  // turned screen, so the loops fit it without writing the numbers in.
  for (int x = 100; x < display.width(); x += 100) {
    for (int y = 0; y < display.height(); y += 8) {
      display.drawPixel(x, y, BLACK);
    }
  }

  // The same for the horizontal lines
  for (int y = 100; y < display.height(); y += 100) {
    for (int x = 0; x < display.width(); x += 8) {
      display.drawPixel(x, y, BLACK);
    }
  }

  // The x numbers along the top edge and the y numbers down the left edge
  for (int x = 0; x < display.width(); x += 100) {
    display.setCursor(x + 4, 4);
    display.print(x);
  }
  for (int y = 100; y < display.height(); y += 100) {
    display.setCursor(4, y + 4);
    display.print(y);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
