/**
 **************************************************
 * @file        Inkplate6_Show_Pictures_From_Web.ino
 * @brief       Picture from the web example for Soldered
 *              Inkplate 6.
 *
 * @details     Download a picture from the web and show it on
 *              the Inkplate 6, in eight levels of gray.
 *              Connection: none. Needs a Wi-Fi network.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable, Wi-Fi network
 * - Extra:      Wi-Fi network
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Enter the name and the password of your Wi-Fi network in the
 *    ssid and pass lines at the top of the sketch.
 * 3) Upload the sketch to Inkplate 6.
 * 4) The picture appears on the display in eight levels of
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

// Pull in the libraries: Inkplate, the font file that sits next to this sketch, and Wi-Fi
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include <WiFi.h>

// The name and the password of your Wi-Fi network
#define ssid ""
#define pass ""

// The web address of the picture. It has to point straight at a JPG, PNG or BMP file.
const char *pictureUrl = "https://learn.soldered.com/inkplate-6/files/coastal-town.jpg";

// Gray mode: eight levels, 0 (black) to 7 (white). Pictures need it to look right.
Inkplate display(INKPLATE_3BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(50, 50);

  // Join the Wi-Fi network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Give the connection a moment to settle before the download
  delay(2000);

  // Download the picture and draw it with its top left corner at 0, 0, which fills the screen,
  // because the picture is 800 x 600 pixels. The last value, true, turns on dithering. If the
  // download or the drawing fails, say so.
  if (!display.image.draw(pictureUrl, 0, 0, true)) {
    display.println("Could not download the picture");
  }

  // Send the frame buffer to the screen, then turn the Wi-Fi off to save power
  display.display();
  WiFi.mode(WIFI_OFF);
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
