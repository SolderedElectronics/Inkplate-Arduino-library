/**
 **************************************************
 * @file        Inkplate10_microSD_Pictures.ino
 * @brief       Show a picture from the microSD card on Soldered
 *              Inkplate 10.
 *
 * @details     Shows a picture from the microSD card on the Inkplate
 *              10 in eight levels of gray. The picture cosmic-
 *              cliffs.jpg (1200 x 825 pixels, NASA / ESA / CSA /
 *              STScI, public domain) is drawn at the top left corner
 *              of the screen.
 *
 *              Copy cosmic-cliffs.jpg from this example folder to the
 *              root of a FAT32 formatted microSD card. Maximum
 *              supported resolution is 1200 x 825 pixels. Expected
 *              output is the picture on the Inkplate display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, microSD card
 * - Extra:      cosmic-cliffs.jpg on the microSD card; SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Copy cosmic-cliffs.jpg to the microSD card and insert the card
 *    into the slot.
 * 3) Upload the sketch to Inkplate 10.
 * 4) The picture appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Gray mode: eight levels, 0 (black) to 7 (white). Pictures need it to look right.
Inkplate display(INKPLATE_3BIT);

// The name of the picture on the card. It can be a JPG, PNG or BMP file.
const char *pictureName = "cosmic-cliffs.jpg";

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
  // picture is 1200 x 825 pixels. The last value, true, turns on dithering, which makes the grays look
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
