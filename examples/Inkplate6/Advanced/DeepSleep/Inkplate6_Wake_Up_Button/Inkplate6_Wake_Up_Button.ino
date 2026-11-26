/**
 **************************************************
 * @file        Inkplate6_Wake_Up_Button.ino
 * @brief       Deep sleep with a timer or button wake-up for
 *              Soldered Inkplate 6.
 *
 * @details     Show the wake-up count and reason on the
 *              Inkplate 6, then sleep. Wakes on a 30 second
 *              timer or when the WakeUp button is pressed.
 *              Connection: none, the button is on the board.
 *              Power and upload over USB-C.
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
 * 3) The count and the reason for the wake-up appear on the
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

// Microseconds in one second
#define uS_TO_S_FACTOR 1000000ULL

// Seconds to sleep if nobody presses the button
#define TIME_TO_SLEEP 30

Inkplate display(INKPLATE_1BIT);

// Survives deep sleep, like in T9
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
  display.setTextSize(2);

  display.setCursor(100, 240);
  display.print("Wake-up number ");
  display.println(wakeCount);

  display.setCursor(100, 300);
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
