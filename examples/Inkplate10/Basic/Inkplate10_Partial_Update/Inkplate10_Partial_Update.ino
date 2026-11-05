/**
 **************************************************
 * @file        Inkplate10_Partial_Update.ino
 * @brief       Partial update text scrolling example for Soldered
 *              Inkplate 10.
 *
 * @details     Demonstrates how to use partial update on the Inkplate
 *              10 e-paper display in 1-bit (black & white) mode. The
 *              example scrolls a line of text across the screen by
 *              updating only the changed area with partialUpdate(),
 *              and forces a full refresh every few updates to keep
 *              the picture quality good.
 *
 *              Partial update is available only in 1-bit (black &
 *              white) mode and is not recommended for the first
 *              refresh after power-up. Perform a full refresh every
 *              5-10 partial updates to maintain good picture quality.
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
 * 3) The text scrolls across the display using partial updates.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Josip Šimun Kuči @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select Soldered Inkplate 10 in the boards menu."
#endif

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// The text that scrolls
const char text[] = "Partial updates!";

// Where the text starts on the x axis. It begins left of the screen and slides in.
int offset = -320;

void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black letters on white, in the font from T1, twice its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(2);

  // Do not wrap text onto the next line when it reaches the edge
  display.setTextWrap(false);

  // Make every 30th update a full refresh, to clear the leftovers
  display.setFullUpdateThreshold(29);
}

void loop() {
  // Redraw the text in its new place
  display.clearDisplay();
  display.setCursor(offset, 360);
  display.print(text);

  // Update only what changed. The second argument keeps the screen's power
  // supply on between updates, which makes the next update faster.
  display.partialUpdate(false, true);

  // Move the text 15 pixels to the right for the next round
  offset += 15;

  // When it has left the screen, bring it back to the left
  if (offset > 1200) {
    offset = -320;
  }

  // Wait half a second between updates
  delay(500);
}
