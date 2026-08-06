/**
 **************************************************
 * @file        Inkplate10_RTC_Timer.ino
 * @brief       RTC timer functionality example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to use the PCF85063A real-time clock (RTC)
 *              timer functionality on the Inkplate 10 board. The example shows
 *              how to set time and date, configure the RTC timer, read current
 *              time values, and display them on the e-paper screen using
 *              partial updates.
 *
 *              Inkplate 10 uses the PCF85063A RTC chip. Partial update works
 *              only in 1-bit (black & white) mode and is not recommended for
 *              the first refresh after power-up; perform a full refresh every
 *              5-10 partial updates to maintain display quality. Expected
 *              output is the current date and time on the Inkplate display,
 *              with the timer operating according to the configured interval.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) Initialize RTC time and date if not already configured.
 * 4) Configure the RTC timer in the code.
 * 5) The timer event is handled while current time is read and displayed.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-11-12
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

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
int countdown_time = 15;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5); // Set text to be 5 times bigger than classic 5x7 px text

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

// Variable that keeps count on how much screen has been partially updated
int n = 0;
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

    delay(700);                             // Delay between refreshes.
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
