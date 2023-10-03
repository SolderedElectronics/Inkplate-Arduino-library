/*
   Inkplate6PLUS_RTC_Alarm example for Soldered Inkplate 6Plus
   For this example you will need USB cable and Inkplate 6Plus.
   Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   In this example we will show how to use basic alarm and clock functions of PCF85063 RTC on Inkplate board.
   This example will show how to set time and date, how to set alarm, how to read time and how to print time on Inkplate
   using partial updates. NOTE: Partial update is only available on 1 Bit mode (BW) and it is not recommended to use it
   on first refresh after power up. It is recommended to do a full refresh every 5-10 partial refresh to maintain good
   picture quality.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   20 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
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
uint8_t weekday = 1;
uint8_t day = 20;
uint8_t month = 2;
uint8_t year = 22;

// Set alarm time and date (alarm will be generated 10 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 50;
uint8_t alarmSeconds = 40;
uint8_t alarmWeekday = 1;
uint8_t alarmDay = 20;

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5); // Set text to be 5 times bigger than classic 5x7 px text

    display.rtcSetTime(hour, minutes, seconds);                                         // Send time to RTC
    display.rtcSetDate(weekday, day, month, year);                                      // Send date to RTC
    display.rtcSetAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm
}

// Variable that keeps count on how much screen has been partially updated
int n = 0;
void loop()
{
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
        display.setCursor(100, 300);                                  // Set position of the text
        printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

        if (display.rtcCheckAlarmFlag()) // Check if alarm has occurred
        {
            display.rtcClearAlarmFlag(); // It's recommended to clear alarm flag after alarm has occurred
            display.setCursor(400, 400); // Set new position for cursor
            display.print("ALARM!");
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

        time1 = millis(); // Store current millis
    }
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint16_t _month,
               uint8_t _year)
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
