/**
 **************************************************
 * @file        Inkplate6_Fonts.ino
 * @brief       Two fonts example for Soldered Inkplate 6.
 *
 * @details     Show the same text in two fonts on the
 *              Inkplate 6. Connection: none, the screen is on
 *              the board. Power and upload over USB-C.
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
 * 3) The same text appears twice, once in each font.
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

// Pull in the Inkplate library, then the two font files that sit next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include "FreeSerif_24.h"

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);

  // The first font: a label at the normal size, then the text three times bigger
  display.setFont(&SourceSans3_24);
  display.setTextSize(1);
  display.setCursor(50, 50);
  display.print("Source Sans, the font from T1");
  display.setTextSize(3);
  display.setCursor(50, 100);
  display.print("Hello World!");

  // The second font: the same two lines, lower on the screen
  display.setFont(&FreeSerif_24);
  display.setTextSize(1);
  display.setCursor(50, 300);
  display.print("FreeSerif, a second font");
  display.setTextSize(3);
  display.setCursor(50, 350);
  display.print("Hello World!");

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
