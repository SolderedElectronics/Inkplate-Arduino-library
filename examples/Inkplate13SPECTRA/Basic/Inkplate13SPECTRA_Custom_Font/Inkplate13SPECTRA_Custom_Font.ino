/**
 **************************************************
 * @file        Inkplate13SPECTRA_Custom_Font.ino
 * @brief       Write a line in the default font and a line in the FreeSerif font
 *              on the Inkplate 13, to compare them.
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

// Pull in the Inkplate library, then the serif font file that sits next to this sketch
#include "Inkplate.h"
#include "FreeSerif24pt7b.h"

// The display object
Inkplate display;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setTextColor(INKPLATE_BLACK);

  // The first line, in the default font. Size 3 makes its letters three times bigger.
  display.setTextSize(3);
  display.setCursor(180, 180);
  display.print("Default font");

  // Select the serif font. It is already big, so size 1 is enough.
  display.setFont(&FreeSerif24pt7b);
  display.setTextSize(1);
  display.setCursor(180, 330);
  display.print("Serif font!");

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
