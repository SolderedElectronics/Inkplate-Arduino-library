/*
   Inkplate4_RTC_Alarm_With_Deep_Sleep example for Soldered Inkplate 4
   For this example you will need only a USB-C cable and Inkplate 4.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to use an RTC alarm interrupt with deep sleep.
   Inkplate features an RTC chip with interrupt for alarm connected to GPIO39.
   Inkplate board will wake up every 10 seconds, refresh the screen and go back to sleep.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   5 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Create an object on Inkplate library
Inkplate display;

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Clear alarm flag from any previous alarm
    display.rtcClearAlarmFlag();

    // Check if RTC is already set. If it's not, set a time and date
    if (!display.rtcIsSet())
    {
        // display.setTime(hour, minute, sec);
        display.rtcSetTime(6, 30, 00); // 24H mode, ex. 6:30:00
        // display.setDate(weekday, day, month, yr);
        display.rtcSetDate(3, 5, 4, 2023); // 0 for Sunday, 1 for Monday, ... for example Wensday, 5.4.2023.

        // display.rtcSetEpoch(1680676200); // Or use epoch for setting the time and date
    }

    // Display current time and date
    printCurrentTime();
    display.display();

    // Set RTC alarm 10 seconds from now
    display.rtcSetAlarmEpoch(display.rtcGetEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    // Put ESP32 into deep sleep. Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Print the current time on the screen
void printCurrentTime()
{
    // Set cursor and font size
    display.setCursor(25, 140);
    display.setTextSize(2);

    // Get data from the RTC
    display.rtcGetRtcData();

    // Find a weekday and print its full name
    switch (display.rtcGetWeekday())
    {
    case 0:
        display.print("Sunday, ");
        break;
    case 1:
        display.print("Monday, ");
        break;
    case 2:
        display.print("Tuesday, ");
        break;
    case 3:
        display.print("Wednesday, ");
        break;
    case 4:
        display.print("Thursday, ");
        break;
    case 5:
        display.print("Friday, ");
        break;
    case 6:
        display.print("Saturday, ");
        break;
    }

    // Print date and time
    display.print(display.rtcGetDay());
    display.print(".");
    display.print(display.rtcGetMonth());
    display.print(".");
    display.print(display.rtcGetYear());
    display.print(". ");
    print2Digits(display.rtcGetHour());
    display.print(':');
    print2Digits(display.rtcGetMinute());
    display.print(':');
    print2Digits(display.rtcGetSecond());
}

// A function that prints 2 digits
// It adds 0 before the number if it's only one digit
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
