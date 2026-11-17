/**
 **************************************************
 * @file        Inkplate13SPECTRA_SD_Pictures.ino
 * @brief       Show a picture from the microSD card on the Inkplate 13, in
 *              colour.
 *
 * @details     Picture: Cosmic Cliffs, NASA / ESA / CSA / STScI, public domain.
 *              The manual has it cropped to 1600 x 1200, the original is at https
 *              ://commons.wikimedia.org/wiki/File:NASA%E2%80%99s_Webb_Reveals_Cos
 *              mic_Cliffs,_Glittering_Landscape_of_Star_Birth.jpg Connection: a
 *              microSD card in the slot on the board, with a file called cosmic-
 *              cliffs.jpg on it.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, microSD card
 * - Extra:      A microSD card in the slot
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

// The name of the picture on the card. It can be a JPG, PNG or BMP file.
const char *pictureName = "cosmic-cliffs.jpg";

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(2);
  display.setCursor(50, 50);

  // Start the card. If it fails, say so.
  if (!display.sdCardInit()) {
    display.println("SD card error!");
  }
  // Draw the picture with its top left corner at 0, 0. The picture is 1600 x 1200 pixels,
  // so it fills the 1600 x 1200 screen. The last value, true, turns on
  // dithering, which makes the colours look smoother. If this fails, say so.
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
