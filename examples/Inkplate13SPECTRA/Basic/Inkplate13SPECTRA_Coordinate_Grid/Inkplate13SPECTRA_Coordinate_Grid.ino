/**
 **************************************************
 * @file        Inkplate13SPECTRA_Coordinate_Grid.ino
 * @brief       Draw a dotted grid with numbers on the Inkplate 13, to see how the
 *              coordinates map to pixels.
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

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// The display object. Inkplate 13 draws in six colours, and this task uses black and white.
Inkplate display;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE);
  display.setTextSize(1);

  // A vertical dotted line every 100 pixels. drawPixel(x, y, colour) sets one pixel, and
  // a pixel every 8 pixels down the screen makes a dotted line.
  for (int x = 100; x < 1600; x += 100) {
    for (int y = 0; y < 1200; y += 8) {
      display.drawPixel(x, y, INKPLATE_BLACK);
    }
  }

  // The same for the horizontal lines
  for (int y = 100; y < 1200; y += 100) {
    for (int x = 0; x < 1600; x += 8) {
      display.drawPixel(x, y, INKPLATE_BLACK);
    }
  }

  // The x numbers along the top edge, one every 100 pixels, and the y numbers down the
  // left edge. Each number starts a few pixels right of and below the position it names.
  for (int x = 0; x < 1600; x += 100) {
    display.setCursor(x + 4, 4);
    display.print(x);
  }
  for (int y = 100; y < 1200; y += 100) {
    display.setCursor(4, y + 4);
    display.print(y);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
