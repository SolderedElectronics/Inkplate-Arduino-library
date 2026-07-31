/**
 **************************************************
 * @file        Inkplate10_RTC_Interrupt_Alarm.ino
 * @brief       RTC alarm with interrupt example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to use the PCF85063A real-time clock (RTC)
 *              alarm functionality together with its interrupt output on
 *              Inkplate 10. The example shows how to set time and date,
 *              configure an alarm, read current time, print it on the display
 *              using partial updates, and handle the RTC interrupt event.
 *
 *              Inkplate 10 uses the PCF85063A RTC chip. Partial update works
 *              only in 1-bit (black & white) mode and is not recommended for
 *              the first refresh after power-up; perform a full refresh every
 *              5-10 partial updates to maintain display quality. Expected
 *              output is the current date and time on the Inkplate display,
 *              with the alarm interrupt triggered at the configured time.
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
 * 3) Initialize RTC time and date if not already set.
 * 4) Configure the RTC alarm and enable interrupt handling.
 * 5) When the alarm triggers, the interrupt is handled in software.
 * 6) Current time and alarm status are displayed on the screen.
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

#include "Inkplate.h"             // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT);  // Create an object on Inkplate library and also set library into 1-bit mode (BW)

volatile int _alarmFlag = 0;      // Variable to store alarm flag

void IRAM_ATTR alarmISR()         // This function will be called when alarm interrupt event happens
{                                 // NOTE: Function must be above setup() and loop()!
  _alarmFlag = 1;                 // Set alarm flag
}

void setup()
{
    pinMode(39, INPUT_PULLUP);      // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5); // Set text to be 5 times bigger than classic 5x7 px text
  
    display.rtc.setEpoch(1589610300);
    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // display.rtcSetTime(6, 25, 0);        // Or you can use other way to set the time and date
    // display.rtcSetDate(6, 16, 5, 2020);
    // display.rtcSetAlarm(10, 25, 6, 16, 6); // Set alarm 10 seconds from now
  
    attachInterrupt(39, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

// Variable that keeps count on how much screen has been partially updated
int n = 0;
void loop()
{
    display.clearDisplay();         // Clear frame buffer of display
    display.setCursor(100, 100);    // Set position of the text
    display.rtc.getRtcData();          // Get the time and date from RTC

    // Print the time on screen
    printTime(display.rtc.getHour(), display.rtc.getMinute(), display.rtc.getSecond(), display.rtc.getDay(), display.rtc.getWeekday(), display.rtc.getMonth(), display.rtc.getYear());
    
    if (_alarmFlag)     // Check alarm flag
    {
        // _alarmFlag = 0;              // Uncomment if you want to clear this flag
        display.rtc.clearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
        display.setCursor(200, 200);    // Set position of the text
        display.print("ALARM");         // Print text
    }
    
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
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
