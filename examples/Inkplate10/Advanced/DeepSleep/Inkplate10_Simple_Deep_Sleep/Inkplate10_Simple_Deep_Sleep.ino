/**
 **************************************************
 * @file        Inkplate10_Simple_Deep_Sleep.ino
 * @brief       Timer deep sleep wake-up counter example for Soldered
 *              Inkplate 10.
 *
 * @details     Demonstrates low-power operation on Inkplate 10 using
 *              ESP32 deep sleep. On each wake-up the board shows a
 *              wake-up counter stored in RTC memory, refreshes the
 *              display and goes back to sleep for 20 seconds.
 *
 *              Deep sleep restarts the program from the beginning on
 *              every wake-up, and normal RAM is lost. A variable
 *              marked with RTC_DATA_ATTR is kept across deep sleep.
 *              Expected output is a counter that grows by one every
 *              20 seconds.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable (or battery for low-power testing)
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) The board shows the counter and enters deep sleep for 20
 *    seconds, then repeats.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Image converter tool:
 *              https://tools.soldered.com/tools/image-converter/
 *
 * @author      Josip Šimun Kuči @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select Soldered Inkplate 10 in the boards menu."
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

Inkplate display(INKPLATE_1BIT);

// Everything happens in setup(), because every wake-up starts the sketch again
void setup() {
  display.begin();

  // Count this wake-up
  wakeCount++;

  // Draw the counter
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
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
