/**
 **************************************************
 * @file        Inkplate13SPECTRA_Full_Screen_Colors.ino
 * @brief       Fill the whole Inkplate 13 screen with its six colours, as six
 *              vertical bars.
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
  display.begin();
  display.clearDisplay();

  // One bar for each colour. The colours have the numbers 0 to 5: black, white,
  // yellow, red, blue and green.
  for (int colour = 0; colour < 6; colour++) {
    // A solid rectangle: left edge, top edge, width, height, then the colour number.
    // Every bar is a sixth of the screen wide, plus 2 pixels so that no gap is left.
    display.fillRect(colour * 1600 / 6, 0, 1600 / 6 + 2, 1200, colour);
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
