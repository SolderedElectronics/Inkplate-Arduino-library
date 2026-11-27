/**
 **************************************************
 * @file        Inkplate13SPECTRA_RTC_Set_From_Internet.ino
 * @brief       Set the real-time clock of the Inkplate 13 from the internet, then
 *              show the time.
 *
 * @details     Connection: none. Needs a Wi-Fi network, but only for the first
 *              few seconds.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, Wi-Fi network
 * - Extra:      Wi-Fi network name and password, set in the sketch
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

// The display object
Inkplate display;

// The strip of the screen that holds the clock: top left corner, width and height
const int stripX = 0;
const int stripY = 300;
const int stripWidth = 1600;
const int stripHeight = 600;

// How many updates have been sent so far
int updates = 0;

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
  display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE);
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
}

// loop() runs over and over
void loop() {
  // Read the time and date from the RTC
  display.rtc.getRtcData();

  // Clear the strip, then draw the clock into it
  display.fillRect(stripX, stripY, stripWidth, stripHeight, INKPLATE_WHITE);

  // The time, hours and minutes
  display.setCursor(100, 350);
  printTwoDigits(display.rtc.getHour());
  display.print(':');
  printTwoDigits(display.rtc.getMinute());

  // The weekday
  display.setCursor(100, 540);
  display.print(weekdayNames[display.rtc.getWeekday()]);

  // The date
  display.setCursor(100, 730);
  printTwoDigits(display.rtc.getDay());
  display.print('.');
  printTwoDigits(display.rtc.getMonth());
  display.print(".20");
  printTwoDigits(display.rtc.getYear());

  // Every 60th update is a full refresh. The others send only the strip.
  updates++;
  if (updates % 60 == 0) {
    display.display();
  } else {
    display.displayPartial(stripX, stripY, stripWidth, stripHeight, true);
  }

  // Keep asking the RTC until the minute has changed
  int shownMinute = display.rtc.getMinute();
  while (display.rtc.getMinute() == shownMinute) {
    delay(1000);
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
