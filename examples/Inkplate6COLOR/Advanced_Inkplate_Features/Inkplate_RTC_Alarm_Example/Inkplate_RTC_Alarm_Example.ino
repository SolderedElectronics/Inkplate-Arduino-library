/*
   Inkplate_RTC_Alarm_Example example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   In this example we will show how to use basic alarm and clock functions of PCF85063 RTC on Inkplate board.
   This example will show how to set time and date, how to set alarm, how to read time and how to print time on Inkplate.
   
   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
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
uint8_t minutes = 51;
uint8_t seconds = 0;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 menas Monday, ...)
uint8_t weekday = 5;
uint8_t day = 19;
uint8_t month = 5;
uint8_t year = 22;

// Set alarm time and date (alarm will be generated 60 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 52;
uint8_t alarmSeconds = 0;
uint8_t alarmWeekday = 5;
uint8_t alarmDay = 19;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtcReset();           //  Reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text

    pinMode(39, INPUT_PULLUP);

    display.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
    display.rtcSetDate(weekday, day, month, year); // Send date to RTC
    display.rtcSetAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm
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
    display.setCursor(100, 300);                                  // Set position of the text
    display.setTextColor(INKPLATE_GREEN, INKPLATE_WHITE);         //Set text color and background
    printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

    if (display.rtcCheckAlarmFlag())  // Check if alarm has occurred
    {
      display.rtcClearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
      display.setCursor(400, 400);    // Set new position for cursor
      display.print("ALARM!");
    }

        display.display(true); // Do a full refresh


    delay(60000);  // Delay between refreshes one minute
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
