/**
 **************************************************
 * @file        Inkplate6FLICK_RTC_Interrupt_Alarm.ino
 * @brief       RTC interrupt alarm demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to use the PCF85063A RTC alarm interrupt
 *              functionality on Inkplate 6FLICK. The example sets the current
 *              time using an epoch value, configures an alarm event, and uses
 *              an interrupt on GPIO39 to detect when the alarm occurs.
 *              The current time and date are periodically read from the RTC
 *              and displayed on the Inkplate e-paper display. When the alarm
 *              triggers, a message is shown on the screen.
 *
 *              The RTC interrupt line is connected to ESP32 GPIO39 and the
 *              interrupt service routine (ISR) only sets a volatile flag that is
 *              processed in the main loop. Partial update works only in 1-bit
 *              (black & white) mode, and it is recommended to perform a full
 *              refresh every 5-10 partial updates to maintain good display
 *              quality. Expected output is the current time and date on the
 *              e-paper screen, with "ALARM" text appearing when the RTC alarm
 *              interrupt occurs.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) The RTC time is set using an epoch timestamp.
 * 4) An alarm is scheduled to trigger shortly after startup.
 * 5) When the RTC alarm fires, the interrupt sets a flag and the display shows
 *    an "ALARM" message.
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
  
    display.rtc.setEpoch(1772184403);
    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // display.rtc.setTime(6, 25, 0);        // Or you can use other way to set the time and date
    // display.rtc.setDate(6, 16, 5, 2020);
    // display.rtc.setAlarm(10, 25, 6, 16, 6); // Set alarm 10 seconds from now
  
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
