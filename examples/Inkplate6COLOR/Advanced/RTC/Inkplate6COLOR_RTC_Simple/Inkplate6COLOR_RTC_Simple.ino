/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Simple.ino
 * @brief       Demonstrates basic RTC time/date setup and display on
 *              Inkplate 6COLOR.
 *
 * @details     This example shows how to use the onboard PCF85063(A) real-time
 *              clock on Inkplate 6COLOR for basic clock functionality. The
 *              sketch resets the RTC, sets an initial time and date, reads the
 *              current RTC values, and prints the formatted time and date on
 *              the e-paper display.
 *
 *              After initialization, the example keeps the ESP32 awake and
 *              refreshes the display once per minute (full refresh) using a
 *              millis()-based timing interval. This demonstrates a simple
 *              polling workflow for RTC-based time display without using alarms,
 *              interrupts, timers or deep sleep.
 *
 *              This example is useful as a starting point for clocks, wall
 *              displays, dashboards, and other projects that need basic RTC
 *              timekeeping and periodic screen updates. display.rtc.reset()
 *              clears previous RTC state, so the configured time/date is
 *              reapplied on every reset or power cycle. The PCF85063(A) RTC is
 *              suitable for general timekeeping, but persistence depends on
 *              correct RTC setup and backup power conditions.
 *
 *              Expected output is the current time in HH:MM:SS format followed
 *              by weekday and date.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     not used in this example
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Adjust the initial time/date values in the sketch if needed.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) On startup, the sketch resets the RTC and writes the configured time/date.
 * 5) The current time and date are read from the RTC and rendered on the
 *    display.
 * 6) The screen is refreshed automatically once per minute.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

#define REFRESH_DELAY 60000 // Delay between refreshes one minute
unsigned long time1;        // Time for measuring refresh in millis

// Set clock
uint8_t hour = 12;
uint8_t minutes = 50;
uint8_t seconds = 0;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 1;
uint8_t day = 20;
uint8_t month = 2;
uint8_t year = 23;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtc.reset();     // reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text
    display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE); // Set text color and background

    display.rtc.setTime(hour, minutes, seconds);    // Send time to RTC
    display.rtc.setDate(weekday, day, month, year); // Send date to RTC
    getAndDisplayTime();                           // Display time on the screen
}

void loop()
{
    // Refresh screen every one minute
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        // Display time on the screen
        getAndDisplayTime();

        // Store current millis
        time1 = millis();
    }
}

void getAndDisplayTime()
{
    display.rtc.getRtcData(); // Get the time and date from RTC

    seconds = display.rtc.getSecond();  // Store senconds in a variable
    minutes = display.rtc.getMinute();  // Store minutes in a variable
    hour = display.rtc.getHour();       // Store hours in a variable
    day = display.rtc.getDay();         // Store day of month in a variable
    weekday = display.rtc.getWeekday(); // Store day of week in a variable
    month = display.rtc.getMonth();     // Store month in a variable
    year = display.rtc.getYear();       // Store year in a variable

    display.clearDisplay();                                       // Clear content in frame buffer
    display.setCursor(80, 300);                                   // Set position of the text
    printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen
    display.display();                                            // Refresh the screen
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint8_t _month,
               uint16_t _year)
{
    // Write time and date info on screen
    char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    print2Digits(_hour);
    display.print(':');
    print2Digits(_minutes);
    display.print(':');
    print2Digits(_seconds);

    display.print(' ');

    display.print(wday[_weekday]);
    display.print(", ");
    print2Digits(_day);
    display.print('/');
    print2Digits(_month);
    display.print('/');
    display.print(_year, DEC);
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
