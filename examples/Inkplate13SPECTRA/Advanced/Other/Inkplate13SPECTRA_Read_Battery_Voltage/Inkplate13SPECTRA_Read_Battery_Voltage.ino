/**
 **************************************************
 * @file        Inkplate13SPECTRA_Read_Battery_Voltage.ino
 * @brief       Show the battery voltage of the Inkplate 13 in big text, next to a
 *              battery picture.
 *
 * @details     Connection: none. The measurement is on the board. A battery is
 *              optional.
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

// Pull in the Inkplate library, then the font and the picture files that sit next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include "battSymbol.h"

// The display object
Inkplate display;

// The strip of the screen that shows the voltage: top left corner, width and height
const int stripX = 0;
const int stripY = 500;
const int stripWidth = 1600;
const int stripHeight = 200;

// How many measurements have been shown so far
int updates = 0;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black text in the font from T1, four times its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(4);
}

// loop() runs over and over
void loop() {
  // Take the measurement
  float voltage = display.readBattery();

  // Clear the strip, then draw the picture and write the voltage to the right of it
  display.fillRect(stripX, stripY, stripWidth, stripHeight, INKPLATE_WHITE);
  display.drawBitmap(554, 555, battSymbol, 212, 90, INKPLATE_BLACK);
  display.setCursor(810, stripY + 60);
  display.print(voltage, 2);  // two decimals
  display.print(" V");

  // Every 10th update is a full refresh. The others send only the strip.
  updates++;
  if (updates % 10 == 0) {
    display.display();
  } else {
    display.displayPartial(stripX, stripY, stripWidth, stripHeight, true);
  }

  // Wait 10 seconds before the next measurement
  delay(10000);
}
