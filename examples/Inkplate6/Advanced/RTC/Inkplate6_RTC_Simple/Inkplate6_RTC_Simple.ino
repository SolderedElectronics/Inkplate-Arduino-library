/**
 **************************************************
 * @file        Inkplate6_RTC_Simple.ino
 * @brief       Real-time clock example for Soldered Inkplate
 *              6.
 *
 * @details     Show a clock on the Inkplate 6, using the
 *              real-time clock on the board. The clock is set
 *              to the time at which the sketch was compiled.
 *              Connection: none. The RTC is on the board.
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
 * 3) The time, the weekday and the date appear on the
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

// The time library, to turn the compile date and time into numbers
#include <time.h>

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// The names of the weekdays, in the same order as the numbers: 0 is Sunday
const char *weekdayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.display();

  // Black text on a white background, in the font from T1, five times its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(5);

  // Make every sixtieth update a full refresh, about once a minute, to clear the leftovers
  display.setFullUpdateThreshold(59);

  // The compiler fills __DATE__ and __TIME__ with the moment the sketch was compiled,
  // for example "Nov  2 2026" and "14:05:09". Turn them into numbers.
  struct tm compiled;
  strptime(__DATE__ " " __TIME__, "%b %d %Y %H:%M:%S", &compiled);

  // mktime() works out the weekday from the date
  mktime(&compiled);

  // Send the time and the date to the RTC. The year is the last two digits.
  display.rtc.setTime(compiled.tm_hour, compiled.tm_min, compiled.tm_sec);
  display.rtc.setDate(compiled.tm_wday, compiled.tm_mday, compiled.tm_mon + 1, compiled.tm_year - 100);
}

// loop() runs over and over
void loop() {
  // Read the time and date back from the RTC
  display.rtc.getRtcData();

  display.clearDisplay();

  // The time
  display.setCursor(100, 50);
  printTwoDigits(display.rtc.getHour());
  display.print(':');
  printTwoDigits(display.rtc.getMinute());
  display.print(':');
  printTwoDigits(display.rtc.getSecond());

  // The weekday
  display.setCursor(100, 230);
  display.print(weekdayNames[display.rtc.getWeekday()]);

  // The date
  display.setCursor(100, 410);
  printTwoDigits(display.rtc.getDay());
  display.print('.');
  printTwoDigits(display.rtc.getMonth());
  display.print(".20");
  printTwoDigits(display.rtc.getYear());

  // Send only what changed
  display.partialUpdate(false, true);

  // An update takes a fraction of a second, so a fixed delay would let the clock drift
  // and skip seconds. Instead, keep asking the RTC until the second has changed.
  int shownSecond = display.rtc.getSecond();
  while (display.rtc.getSecond() == shownSecond) {
    display.rtc.getRtcData();
  }
}

// Print a number with a leading zero, so 7 shows as 07
void printTwoDigits(uint8_t value) {
  if (value < 10) {
    display.print('0');
  }
  display.print(value);
}
