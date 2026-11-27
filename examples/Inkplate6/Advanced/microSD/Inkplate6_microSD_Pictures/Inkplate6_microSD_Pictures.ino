/**
 **************************************************
 * @file        Inkplate6_microSD_Pictures.ino
 * @brief       Picture from the microSD card example for
 *              Soldered Inkplate 6.
 *
 * @details     Show a picture from the microSD card on the
 *              Inkplate 6, in eight levels of gray.
 *              Connection: a microSD card in the slot on the
 *              board, with a file called coastal-town.jpg on
 *              it.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable, microSD card
 * - Extra:      microSD card formatted as FAT32, with coastal-town.jpg on it
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6.
 * 3) The picture appears on the display in eight levels of
 *    gray.
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

// Gray mode: eight levels, 0 (black) to 7 (white). Pictures need it to look right.
Inkplate display(INKPLATE_3BIT);

// The name of the picture on the card. It can be a JPG, PNG or BMP file.
const char *pictureName = "coastal-town.jpg";

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(50, 50);

  // Start the card. If it fails, say so.
  if (!display.sdCardInit()) {
    display.println("SD card error!");
  }
  // Draw the picture with its top left corner at 0, 0, which fills the screen, because the
  // picture is 800 x 600 pixels. The last value, true, turns on dithering, which makes the grays look
  // smoother. If this fails, say so.
  else if (!display.image.draw(pictureName, 0, 0, true)) {
    display.print("Could not open ");
    display.println(pictureName);
  }

  // Send the frame buffer to the screen, then let the card sleep
  display.display();
  display.sdCardSleep();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
