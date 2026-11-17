/**
 **************************************************
 * @file        Inkplate13SPECTRA_Partial_Update.ino
 * @brief       Scroll a line of text across a strip of the Inkplate 13 with
 *              partial updates.
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

// The display object
Inkplate display;

// The text that scrolls
const char text[] = "Partial updates!";

// The strip of the screen that changes: top left corner, width and height
const int stripX = 0;
const int stripY = 540;
const int stripWidth = 1600;
const int stripHeight = 120;

// Where the text starts on the x axis. It begins left of the screen and slides in.
int offset = -420;

// How many updates have been sent so far
int updates = 0;

void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black letters in the font from T1, twice its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(2);

  // Do not wrap text onto the next line when it reaches the edge
  display.setTextWrap(false);
}

void loop() {
  // Clear the strip, then draw the text in its new place
  display.fillRect(stripX, stripY, stripWidth, stripHeight, INKPLATE_WHITE);
  display.setCursor(offset, stripY + 30);
  display.print(text);

  // Every 20th update is a full refresh. The others send only the strip.
  updates++;
  if (updates % 20 == 0) {
    display.display();
  } else {
    // The last value keeps the screen's power supply on between updates
    display.displayPartial(stripX, stripY, stripWidth, stripHeight, true);
  }

  // Move the text 40 pixels to the right for the next round
  offset += 40;

  // When it has left the screen, bring it back to the left
  if (offset > 1600) {
    offset = -420;
  }

  // Wait half a second between updates
  delay(500);
}
