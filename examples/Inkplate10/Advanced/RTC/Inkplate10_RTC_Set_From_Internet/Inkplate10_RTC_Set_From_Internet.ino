/**
 **************************************************
 * @file        Inkplate10_RTC_Set_From_Internet.ino
 * @brief       Set the RTC from the internet (NTP) example for
 *              Soldered Inkplate 10.
 *
 * @details     Sets the real-time clock of the Inkplate 10 from the
 *              internet, then shows the time. Wi-Fi is needed only
 *              for the first few seconds, to ask a time server for
 *              the current time; after that the RTC keeps the time on
 *              its own.
 *
 *              Enter the name and the password of your Wi-Fi network
 *              in the two #define lines. Expected output is the
 *              current time on the Inkplate display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      Wi-Fi network, SourceSans3_24.h font file in the sketch folder
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Enter your Wi-Fi name and password, then upload the sketch to
 *    Inkplate 10.
 * 3) The time is set from the internet and appears on the display.
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

// Pull in the Inkplate library, the font file that sits next to this sketch, the Wi-Fi library and the time library
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include <WiFi.h>
#include <time.h>

// The name and the password of your Wi-Fi network
#define ssid ""
#define pass ""

// The time zone. This one is central Europe, with the summer time change.
const char *timeZone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// The names of the weekdays, in the same order as the numbers: 0 is Sunday
const char *weekdayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Stop with a message on the screen
void fail(const char *message) {
  display.println(message);
  display.display();
  while (true);
}

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();

  // Black text on a white background, in the font from T1, five times its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(5);
  display.setCursor(100, 200);

  // Join the Wi-Fi network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Ask the internet for the time. The first value is the time zone, the others are
  // servers that know the exact time. This is called NTP.
  configTzTime(timeZone, "pool.ntp.org", "time.google.com");

  // Wait until the board has the time, for up to 10 seconds
  struct tm now;
  if (!getLocalTime(&now, 10000)) {
    fail("No time from the internet");
  }

  // The clock is set, so the Wi-Fi is not needed any more. Turn it off to save power.
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Send the time and the date to the RTC. The year is the last two digits.
  display.rtc.setTime(now.tm_hour, now.tm_min, now.tm_sec);
  display.rtc.setDate(now.tm_wday, now.tm_mday, now.tm_mon + 1, now.tm_year - 100);

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Make every sixtieth update a full refresh, about once a minute, to clear the leftovers
  display.setFullUpdateThreshold(59);
}

// loop() runs over and over
void loop() {
  // Read the time and date from the RTC
  display.rtc.getRtcData();

  display.clearDisplay();

  // The time
  display.setCursor(100, 200);
  printTwoDigits(display.rtc.getHour());
  display.print(':');
  printTwoDigits(display.rtc.getMinute());
  display.print(':');
  printTwoDigits(display.rtc.getSecond());

  // The weekday
  display.setCursor(100, 340);
  display.print(weekdayNames[display.rtc.getWeekday()]);

  // The date
  display.setCursor(100, 480);
  printTwoDigits(display.rtc.getDay());
  display.print('.');
  printTwoDigits(display.rtc.getMonth());
  display.print(".20");
  printTwoDigits(display.rtc.getYear());

  // Send only what changed
  display.partialUpdate(false, true);

  // Keep asking the RTC until the second has changed
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
