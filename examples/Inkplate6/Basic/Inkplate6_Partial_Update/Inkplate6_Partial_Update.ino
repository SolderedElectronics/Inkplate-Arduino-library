/**
 **************************************************
 * @file        Inkplate6_Partial_Update.ino
 * @brief       Scrolling text with partial updates for
 *              Soldered Inkplate 6.
 *
 * @details     Scroll a line of text across the Inkplate 6
 *              with partial updates. Connection: none, the
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
 * 3) The text scrolls across the display.
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
  display.setCursor(offset, 260);
  display.print(text);

  // Update only what changed. The second argument keeps the screen's power
  // supply on between updates, which makes the next update faster.
  display.partialUpdate(false, true);

  // Move the text 15 pixels to the right for the next round
  offset += 15;

  // When it has left the screen, bring it back to the left
  if (offset > 800) {
    offset = -320;
  }

  // Wait half a second between updates
  delay(500);
}
