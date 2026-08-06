/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Timer.ino
 * @brief       Demonstrates the PCF85063(A) RTC countdown timer on
 *              Inkplate 6COLOR.
 *
 * @details     This example shows how to use the RTC countdown timer on
 *              Inkplate 6COLOR. The sketch resets the RTC, sets the time and
 *              date, and starts a 15-second countdown timer with
 *              display.rtc.timerSet(RTC::TIMER_CLOCK_1HZ, countdown_time, true,
 *              false).
 *
 *              The available timer clock sources determine the possible
 *              countdown range:
 *              - Inkplate::TIMER_CLOCK_4096HZ   -> min 244 us,  max 62.256 ms
 *              - Inkplate::TIMER_CLOCK_64HZ     -> min 15.625 ms, max 3.984 s
 *              - RTC::TIMER_CLOCK_1HZ           -> min 1 s,     max 255 s
 *              - Inkplate::TIMER_CLOCK_1PER60HZ -> min 60 s,    max 4 h 15 min
 *
 *              The main loop reads and displays the current RTC time and checks
 *              the timer flag with display.rtc.checkTimerFlag(). When the timer
 *              event occurs, the flag is cleared with clearTimerFlag() and the
 *              timer is disabled with disableTimer() to make it a one-shot; keep
 *              disableTimer() commented out if you want the timer to repeat.
 *
 *              The interrupt behavior is configurable too: the timer interrupt
 *              can generate a pulse or follow the timer flag.
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
 * 2) Adjust the initial time/date and countdown_time in the sketch if needed.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) The display shows the current RTC time and date.
 * 5) After the countdown elapses, the timer flag is detected, cleared and the
 *    timer is disabled.
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
    display.rtc.reset();           //  reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         //Set text color and background

    pinMode(39, INPUT_PULLUP); // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.rtc.setTime(hour, minutes, seconds);    // Send time to RTC
    display.rtc.setDate(weekday, day, month, year); // Send date to RTC

    // Set up a timer
    /*   source_clock
     *       Inkplate::TIMER_CLOCK_4096HZ     -> clk = 4096Hz -> min timer = 244uS -> MAX timer = 62.256mS
     *       Inkplate::TIMER_CLOCK_64HZ       -> clk = 64Hz   -> min timer = 15.625mS -> MAX timer = 3.984s
     *       RTC::TIMER_CLOCK_1HZ        -> clk = 1Hz    -> min timer = 1s -> MAX timer = 255s
     *       Inkplate::TIMER_CLOCK_1PER60HZ   -> clk = 1/60Hz -> min timer = 60s -> MAX timer = 4h15min
     *   value
     *       coundowntime in seconds
     *   int_enable
     *       true = enable interrupt; false = disable interrupt
     *   int_pulse
     *       true = interrupt generate a pulse; false = interrupt follows timer flag
     */
    display.rtc.timerSet(RTC::TIMER_CLOCK_1HZ, countdown_time, true, false);
}

void loop()
{
    display.rtc.getRtcData();             // Get the time and date from RTC
    seconds = display.rtc.getSecond();  // Store senconds in a variable
    minutes = display.rtc.getMinute();  // Store minutes in a variable
    hour = display.rtc.getHour();       // Store hours in a variable
    day = display.rtc.getDay();         // Store day of month in a variable
    weekday = display.rtc.getWeekday(); // Store day of week in a variable
    month = display.rtc.getMonth();     // Store month in a variable
    year = display.rtc.getYear();       // Store year in a variable

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

    if (display.rtc.checkTimerFlag())  // Check if timer event has occurred
    {
      display.rtc.clearTimerFlag();  // It's recommended to clear timer flag after timer has occurred
      display.rtc.disableTimer();    // Disable timer if you want to make it one time only. Is you want to be repeatable, comment this line
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
