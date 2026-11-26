/**
 **************************************************
 * @file        Inkplate6_Hello_World.ino
 * @brief       Basic "Hello World" example for Soldered
 *              Inkplate 6.
 *
 * @details     Show "Hello World!" on the Inkplate 6 e-paper
 *              display. Connection: none, the screen is on
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
 * 3) After initialization, "Hello World!" appears on the
 *    display.
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

// Create the display object. INKPLATE_1BIT means black and white only.
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  // Start the display hardware. Call this once, at the start.
  display.begin();

  // Empty the frame buffer. The screen itself does not change yet.
  display.clearDisplay();

  // Where the text starts: 10 pixels from the left, 10 from the top
  display.setCursor(10, 10);

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
