/*
   Inkplate4_RTC_Alarm example for Soldered Inkplate 4
   For this example you will need a USB-C cable and Inkplate 4.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   In this example we will show how to use basic alarm and clock functions of PCF85063 RTC on Inkplate board.
   This example will show how to set time and date, how to set alarm, how to read time and how to print time on
   Inkplate.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   5 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

#define REFRESH_DELAY 10000 // Delay between refreshes
unsigned long time1;        // Time for measuring refresh in millis

// Set clock
uint8_t hours = 12;
uint8_t minutes = 50;
uint8_t seconds = 30;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 4;
uint8_t day = 5;
uint8_t month = 4;
uint8_t year = 23;

// Set alarm time and date (alarm will be generated 20 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 50;
uint8_t alarmSeconds = 50;
uint8_t alarmWeekday = 4;
uint8_t alarmDay = 5;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Set text to be 2 times bigger than classic 5x7 px text

    display.rtcSetTime(hours, minutes, seconds);                                        // Send time to RTC
    display.rtcSetDate(weekday, day, month, year);                                      // Send date to RTC
    display.rtcSetAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm

    // Print time on the screen and remember time when it's printed
    checkAlarmAndShowTime();
    time1 = millis();
}

void loop()
{
    // Every 10 secs refresh the screen, show time and check alarm
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        // Print time and check alarm - the function defined below
        checkAlarmAndShowTime();

        // Store current millis
        time1 = millis();
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

// A function that prints 2 digits
// It adds 0 before the number if it's only one digit
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}

// A function that gets time from RTC and shows it on the screen. It also checks the alarm flag
void checkAlarmAndShowTime()
{
    display.rtcGetRtcData();           // Get the time and date from RTC
    seconds = display.rtcGetSecond();  // Store senconds in a variable
    minutes = display.rtcGetMinute();  // Store minutes in a variable
    hours = display.rtcGetHour();      // Store hours in a variable
    weekday = display.rtcGetWeekday(); // Store day of week in a variable
    day = display.rtcGetDay();         // Store day of month in a variable
    month = display.rtcGetMonth();     // Store month in a variable
    year = display.rtcGetYear();       // Store year in a variable

    display.clearDisplay();                                        // Clear content in frame buffer
    display.setCursor(35, 140);                                    // Set position of the text
    printTime(hours, minutes, seconds, day, weekday, month, year); // Print the time on screen

    if (display.rtcCheckAlarmFlag()) // Check if alarm has occurred
    {
        display.rtcClearAlarmFlag(); // It's recommended to clear alarm flag after alarm has occurred
        display.setCursor(165, 160); // Set new position for cursor
        display.print("ALARM!");

        // Do whatever alarm should do here
    }

    // Show the time on the screen
    display.display();
}
