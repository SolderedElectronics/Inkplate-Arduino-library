/**
 **************************************************
 * @file        Inkplate13SPECTRA_Hello_World.ino
 * @brief       Show "Hello World!" on the Inkplate 13 e-paper display.
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

// Create the display object
Inkplate display;

// setup() runs once, right after the board powers on or resets
void setup() {
  // Start the display hardware. Call this once, at the start.
  display.begin();

  // Empty the frame buffer. The screen itself does not change yet.
  display.clearDisplay();

  // Where the text starts: 10 pixels from the left, 10 from the top
  display.setCursor(10, 10);

  // Inkplate 13 writes in white until you pick a colour, so pick black
  display.setTextColor(INKPLATE_BLACK);

  // Use the font from the file. Its letters are 24 pixels tall.
  display.setFont(&SourceSans3_24);

  // Size 3 makes every pixel of a letter a 3 x 3 block, so the letters are three times bigger
  display.setTextSize(3);

  // Write the text into the frame buffer
  display.print("Hello World!");

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
