/*
   Inkplate6COLOR_RTC_Timer example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   In this example we will show how to use PCF85063A RTC Timer functionality.
   This example will show how to set time and date, how to set up a timer, how to read time and how to print time on Inkplate using partial updates.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   19 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display; // Create an object on Inkplate library

// Set clock
uint8_t hour = 12;
uint8_t minutes = 50;
uint8_t seconds = 30;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 4;
uint8_t day = 11;
uint8_t month = 11;
uint8_t year = 21;

// Set up a 15 seconds timer
int countdown_time = 60;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtcReset();           //  Reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         //Set text color and background

    pinMode(39, INPUT_PULLUP); // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
    display.rtcSetDate(weekday, day, month, year); // Send date to RTC

    // Set up a timer
    /*   source_clock
     *       Inkplate::TIMER_CLOCK_4096HZ     -> clk = 4096Hz -> min timer = 244uS -> MAX timer = 62.256mS
     *       Inkplate::TIMER_CLOCK_64HZ       -> clk = 64Hz   -> min timer = 15.625mS -> MAX timer = 3.984s
     *       Inkplate::TIMER_CLOCK_1HZ        -> clk = 1Hz    -> min timer = 1s -> MAX timer = 255s
     *       Inkplate::TIMER_CLOCK_1PER60HZ   -> clk = 1/60Hz -> min timer = 60s -> MAX timer = 4h15min
     *   value
     *       coundowntime in seconds
     *   int_enable
     *       true = enable interrupt; false = disable interrupt
     *   int_pulse
     *       true = interrupt generate a pulse; false = interrupt follows timer flag
     */
    display.rtcTimerSet(Inkplate::TIMER_CLOCK_1HZ, countdown_time, true, false);
}

void loop()
{
    display.rtcGetRtcData();             // Get the time and date from RTC
    seconds = display.rtcGetSecond();  // Store senconds in a variable
    minutes = display.rtcGetMinute();  // Store minutes in a variable
    hour = display.rtcGetHour();       // Store hours in a variable
    day = display.rtcGetDay();         // Store day of month in a variable
    weekday = display.rtcGetWeekday(); // Store day of week in a variable
    month = display.rtcGetMonth();     // Store month in a variable
    year = display.rtcGetYear();       // Store year in a variable

    display.clearDisplay();                                       // Clear content in frame buffer
    display.setCursor(60, 300);                                  // Set position of the text
    display.setTextColor(INKPLATE_YELLOW, INKPLATE_WHITE); //Set text color and background
    printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

    display.display(); // Do a full refresh
    
    delay(60000);                             // Delay between refreshes.
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

    if (display.rtcCheckTimerFlag())  // Check if timer event has occurred
    {
      display.rtcClearTimerFlag();  // It's recommended to clear timer flag after timer has occurred
      display.rtcDisableTimer();    // Disable timer if you want to make it one time only. Is you want to be repeatable, comment this line
      display.setCursor(400, 400);  // Set new position for cursor
      display.print("Timer!");
    }
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
