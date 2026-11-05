/**
 **************************************************
 * @file        Inkplate10_Wake_Up_Button.ino
 * @brief       Wake-up button and timer deep sleep example for
 *              Soldered Inkplate 10.
 *
 * @details     Demonstrates how to wake the ESP32 from deep sleep on
 *              Inkplate 10 using the WakeUp button and a 30 second
 *              timer. The example keeps a wake-up counter in RTC
 *              memory and shows the count and the wake-up reason on
 *              the e-paper display.
 *
 *              Deep sleep restarts the program from the beginning on
 *              every wake-up. Expected output is an incrementing
 *              count on the display together with the wake-up reason
 *              (WakeUp button or timer).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) The board shows the count and the reason, then enters deep
 *    sleep.
 * 4) Press the WakeUp button, or wait 30 seconds for timer wake-up.
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

// Microseconds in one second
#define uS_TO_S_FACTOR 1000000ULL

// Seconds to sleep if nobody presses the button
#define TIME_TO_SLEEP 30

Inkplate display(INKPLATE_1BIT);

// Survives deep sleep, like in T7
RTC_DATA_ATTR int wakeCount = 0;

void setup() {
  display.begin();
  wakeCount++;

  // Draw the count and the reason
  showInfo();

  // Wake after 30 seconds...
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // ...or when GPIO 36, the WakeUp button, goes LOW
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW);

  // Go to sleep. The sketch ends here.
  esp_deep_sleep_start();
}

void loop() {
}

// Write the wake-up count and the reason onto the screen
void showInfo() {
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
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
