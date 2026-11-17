/**
 **************************************************
 * @file        Inkplate13SPECTRA_Simple_Deep_Sleep.ino
 * @brief       Show a wake-up counter on the Inkplate 13, then sleep for 20
 *              seconds.
 *
 * @details     Connection: none, the screen is on the board. Power and upload
 *              over USB-C.
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

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Microseconds in one second, to turn seconds into the unit the timer wants
#define uS_TO_S_FACTOR 1000000ULL

// How long to sleep, in seconds
#define TIME_TO_SLEEP 20

// This variable lives in RTC memory, so it keeps its value through deep sleep
RTC_DATA_ATTR int wakeCount = 0;

Inkplate display;

// Everything happens in setup(), because every wake-up starts the sketch again
void setup() {
  display.begin();

  // Count this wake-up
  wakeCount++;

  // Draw the counter
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(3);
  display.setCursor(100, 300);
  display.print("Wake-up number ");
  display.print(wakeCount);

  // A full refresh. The picture stays on the screen while the board sleeps.
  display.display();

  // Set the wake-up timer, then go to sleep. The sketch ends here.
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

// Never reached. Deep sleep restarts the sketch from the top on every wake-up.
void loop() {
}
