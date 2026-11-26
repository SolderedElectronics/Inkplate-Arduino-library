/**
 **************************************************
 * @file        Inkplate6_Read_Battery_Voltage.ino
 * @brief       Battery voltage example for Soldered Inkplate
 *              6.
 *
 * @details     Show the battery voltage of the Inkplate 6 in
 *              big text, next to a battery picture.
 *              Connection: none. The measurement is on the
 *              board. A battery is optional.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      None, a battery is optional
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6.
 * 3) The battery voltage appears next to the battery picture.
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

// Pull in the Inkplate library, then the font and the picture files that sit next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include "battSymbol.h"

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black text on a white background, in the font from T1, four times its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(4);

  // Make every tenth update a full refresh, to clear the leftovers
  display.setFullUpdateThreshold(9);
}

// loop() runs over and over
void loop() {
  // Take the measurement
  float voltage = display.readBattery();

  // Redraw the screen: the picture, then the number to the right of it
  display.clearDisplay();

  display.image.draw(battSymbol, 151, 255, 212, 90, BLACK);
  display.setCursor(413, 252);
  display.print(voltage, 2);  // two decimals
  display.print(" V");

  // Send only what changed
  display.partialUpdate(false, true);

  // Wait 10 seconds before the next measurement
  delay(10000);
}
