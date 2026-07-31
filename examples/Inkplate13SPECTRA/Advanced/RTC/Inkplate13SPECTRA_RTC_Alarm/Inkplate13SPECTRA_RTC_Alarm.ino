/*
  Inkplate13SPECTRA_RTC_Alarm example for Soldered Inkplate 13SPECTRA
  For this example you will need USB cable and Inkplate 13SPECTRA.
  Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
  Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:
  
  https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

  In this example we will show how to use basic alarm and clock functions of PCF85063 RTC on Inkplate board.
  This example will show how to set time and date, how to set alarm, how to read time and how to print time on Inkplate.
  
  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  2 February 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

Inkplate inkplate;     // Create an object on Inkplate library

// Set clock
uint8_t hour = 12;
uint8_t minutes = 51;
uint8_t seconds = 10;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 1;
uint8_t day = 2;
uint8_t month = 2;
uint8_t year = 26;

// Set alarm time and date (alarm will be generated 60 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 52;
uint8_t alarmSeconds = 10;
uint8_t alarmWeekday = 1;
uint8_t alarmDay = 2;

void setup()
{
  inkplate.begin();         // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.rtc.reset();     // reset RTC if there is some data in it
  inkplate.clearDisplay();  // Clear frame buffer of display
  inkplate.setTextSize(4);  // Set text to be 4 times bigger than classic 5x7 px text
  inkplate.setTextColor(INKPLATE_RED, INKPLATE_WHITE);          // Set text color and background

  inkplate.rtc.setTime(hour, minutes, seconds);    // Send time to RTC
  inkplate.rtc.setDate(weekday, day, month, year); // Send date to RTC
  inkplate.rtc.setAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm
}

void loop()
{
  inkplate.rtc.getRtcData();           // Get the time and date from RTC
  seconds = inkplate.rtc.getSecond();  // Store senconds in a variable
  minutes = inkplate.rtc.getMinute();  // Store minutes in a variable
  hour = inkplate.rtc.getHour();       // Store hours in a variable
  day = inkplate.rtc.getDay();         // Store day of month in a variable
  weekday = inkplate.rtc.getWeekday(); // Store day of week in a variable
  month = inkplate.rtc.getMonth();     // Store month in a variable
  year = inkplate.rtc.getYear();       // Store year in a variable

  inkplate.clearDisplay();                                        // Clear content in frame buffer
  inkplate.setCursor(100, 300);                                   // Set position of the text
  printTime(hour, minutes, seconds, day, weekday, month, year);   // Print the time on screen

  if (inkplate.rtc.checkAlarmFlag())  // Check if alarm has occurred
  {
    inkplate.rtc.clearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
    inkplate.setCursor(400, 400);     // Set new position for cursor
    inkplate.print("ALARM!");
  }

  inkplate.display(); // Do a full refresh

  delay(60000); // Delay between refreshes one minute
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint16_t _month,
               uint8_t _year)
{
  // Write time and date info on screen
  char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  print2Digits(_hour);
  inkplate.print(':');
  print2Digits(_minutes);
  inkplate.print(':');
  print2Digits(_seconds);

  inkplate.print(' ');

  inkplate.print(wday[_weekday]);
  inkplate.print(", ");
  print2Digits(_day);
  inkplate.print('/');
  print2Digits(_month);
  inkplate.print('/');
  inkplate.print(_year, DEC);
}

void print2Digits(uint8_t _d)
{
  if (_d < 10)
      inkplate.print('0');
  inkplate.print(_d, DEC);
}