/*
   Inkplate6COLOR_RTC_Simple example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Example shows how to use basic clock functions of PCF85063A RTC on Inkplate board.
   This example will show how to set time and date, how to read time and how to print time on Inkplate.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   19 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

#define REFRESH_DELAY 60000 // Delay between refreshes one minute
unsigned long time1;       // Time for measuring refresh in millis

// Set clock
uint8_t hour = 12;
uint8_t minutes = 50;
uint8_t seconds = 0;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 menas Monday, ...)
uint8_t weekday = 5;
uint8_t day = 19;
uint8_t month = 5;
uint8_t year = 22;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtcReset();     //  Reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE); // Set text color and background

    display.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
    display.rtcSetDate(weekday, day, month, year); // Send date to RTC
}

void loop()
{
    //Refresh screen every one minute - the clock will appear after one minute
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        display.rtcGetRtcData();           // Get the time and date from RTC
        seconds = display.rtcGetSecond();  // Store senconds in a variable
        minutes = display.rtcGetMinute();  // Store minutes in a variable
        hour = display.rtcGetHour();       // Store hours in a variable
        day = display.rtcGetDay();         // Store day of month in a variable
        weekday = display.rtcGetWeekday(); // Store day of week in a variable
        month = display.rtcGetMonth();     // Store month in a variable
        year = display.rtcGetYear();       // Store year in a variable

        display.clearDisplay();                                       // Clear content in frame buffer
        display.setCursor(80, 300);                                   // Set position of the text
        printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

        display.display(); // Do a full refresh
        time1 = millis();  // Store current millis
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
