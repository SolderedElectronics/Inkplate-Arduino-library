/**
 **************************************************
 * @file        Inkplate6FLICK_RTC_Simple.ino
 * @brief       Basic RTC clock demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates basic clock functionality of the onboard PCF85063A
 *              RTC on Inkplate 6FLICK. The example sets an initial time and
 *              date, periodically reads the RTC, and prints the current time
 *              and date on the e-paper display. Screen updates are performed
 *              using partial refreshes with occasional full refreshes to keep
 *              image quality high.
 *
 *              Partial update is available only in 1-bit (black & white) mode.
 *              Avoid partial update immediately after power-on - use a full
 *              refresh first - and do a full refresh every 5-10 partial
 *              refreshes to maintain quality. This example keeps the e-paper
 *              power supply enabled during partial updates. Expected output is
 *              the time shown as HH:MM:SS plus the weekday and date in a
 *              readable format (e.g. Monday, 11/11/21).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library (RTC support included)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) The sketch sets the RTC time/date defined in the code.
 * 4) The display updates once per second with the current time and date.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

#define REFRESH_DELAY 1000 // Delay between refreshes
unsigned long time1;       // Time for measuring refresh in millis

// Set clock
uint8_t hour = 12;
uint8_t minutes = 50;
uint8_t seconds = 30;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 4;
uint8_t day = 11;
uint8_t month = 11;
uint8_t year = 21;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5); // Set text to be 5 times bigger than classic 5x7 px text

    display.rtc.setTime(hour, minutes, seconds);    // Send time to RTC
    display.rtc.setDate(weekday, day, month, year); // Send date to RTC
}

// Variable that keeps count on how much screen has been partially updated
int n = 0;
void loop()
{
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        display.rtc.getRtcData();           // Get the time and date from RTC
        seconds = display.rtc.getSecond();  // Store senconds in a variable
        minutes = display.rtc.getMinute();  // Store minutes in a variable
        hour = display.rtc.getHour();       // Store hours in a variable
        day = display.rtc.getDay();         // Store day of month in a variable
        weekday = display.rtc.getWeekday(); // Store day of week in a variable
        month = display.rtc.getMonth();     // Store month in a variable
        year = display.rtc.getYear();       // Store year in a variable

        display.clearDisplay();                                       // Clear content in frame buffer
        display.setCursor(100, 300);                                  // Set position of the text
        printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

        if (n > 9) // Check if you need to do full refresh or you can do partial update
        {
            display.display(true); // Do a full refresh
            n = 0;
        }
        else
        {
            display.partialUpdate(false, true); // Do partial update and keep e-papr power supply on
            n++;                                // Keep track on how many times screen has been partially updated
        }

        time1 = millis(); // Store current millis
    }
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
