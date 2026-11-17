/**
 **************************************************
 * @file        Inkplate13SPECTRA_Wake_Up_Button.ino
 * @brief       Show the wake-up count and reason on the Inkplate 13, then sleep.
 *
 * @details     Wakes on a 30 second timer or when the WakeUp button is pressed.
 *              Connection: none, the button is on the board. Power and upload
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

// Microseconds in one second
#define uS_TO_S_FACTOR 1000000ULL

// Seconds to sleep if nobody presses the button
#define TIME_TO_SLEEP 30

Inkplate display;

// Survives deep sleep, like in T8
RTC_DATA_ATTR int wakeCount = 0;

void setup() {
  display.begin();
  wakeCount++;

  // Draw the count and the reason
  showInfo();

  // Wake after 30 seconds...
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // ...or when GPIO 18, the WakeUp button, goes LOW
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, LOW);

  // Go to sleep. The sketch ends here.
  esp_deep_sleep_start();
}

void loop() {
}

// Write the wake-up count and the reason onto the screen
void showInfo() {
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(3);

  display.setCursor(100, 280);
  display.print("Wake-up number ");
  display.println(wakeCount);

  display.setCursor(100, 360);
  display.print("Woken by: ");

  // Ask the ESP32 what woke it up
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

  if (reason == ESP_SLEEP_WAKEUP_EXT0) {
    display.print("WakeUp button");
  } else if (reason == ESP_SLEEP_WAKEUP_TIMER) {
    display.print("timer");
  } else {
    // The first power-up is not a wake-up from sleep
    display.print("power-up");
  }

  // A full refresh
  display.display();
}
