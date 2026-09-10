/*
  Inkplate7SPECTRA_RTC_Interrupt_Alarm example for Soldered Inkplate 7SPECTRA
  For this example you will need USB cable and Inkplate 7SPECTRA.
  Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
  Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:

  /////////////////////////// INSERT BOARD DEF LINK HERE

  In this example we will show how to use PCF85063A RTC Alarm functionality with interrupt.
  This example will show how to set time and date, how to set up a alarm, how to read time, how to print time on Inkplate and how to handle interrupt.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  2 February 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

#include "Inkplate.h"             // Include Inkplate library to the sketch

Inkplate inkplate;                // Create an object on Inkplate library and also set library into 1-bit mode (BW)

volatile int _alarmFlag = 0;      // Variable to store alarm flag

void IRAM_ATTR alarmISR()         // This function will be called when alarm interrupt event happens
{                                 // NOTE: Function must be above setup() and loop()!
  _alarmFlag = 1;                 // Set alarm flag
}

void setup()
{
  pinMode(2, INPUT_PULLUP);      // Set RTC INT pin on ESP32 GPIO2 as input with pullup resistor enabled

  inkplate.begin();           // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.rtc.reset();       // reset RTC if there is some data in it
  inkplate.clearDisplay();    // Clear frame buffer of display
  inkplate.display();         // Put clear image on display
  inkplate.setTextSize(3);    // Set text to be 3 times bigger than classic 5x7 px text

  // Set RTC time and date via Epoch
  inkplate.rtc.setEpoch(1770032087);
  // Set alarm using Epoch
  inkplate.rtc.setAlarmEpoch(inkplate.rtc.getEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS);

  // inkplate.rtc.setTime(12, 40, 30);        // Or you can use other way to set the time and date
  // inkplate.rtc.setDate(1, 2, 2, 2026);
  // inkplate.rtc.setAlarm(50, 40, 12, 2, 1); // Set alarm 20 seconds from now

  attachInterrupt(2, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

void loop()
{
  inkplate.clearDisplay();         // Clear frame buffer of display
  inkplate.setCursor(60, 100);     // Set position of the text
  inkplate.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         // Set text color and background
  inkplate.rtc.getRtcData();          // Get the time and date from RTC

  // Print the time on screen
  printTime(inkplate.rtc.getHour(), inkplate.rtc.getMinute(), inkplate.rtc.getSecond(), inkplate.rtc.getDay(), inkplate.rtc.getWeekday(), inkplate.rtc.getMonth(), inkplate.rtc.getYear());
  
  if (_alarmFlag)     // Check alarm flag
  {
    // _alarmFlag = 0;                  // Uncomment if you want to clear this flag
    inkplate.rtc.clearAlarmFlag();      // It's recommended to clear alarm flag after alarm has occurred
    inkplate.setCursor(200, 200);       // Set position of the text
    inkplate.print("ALARM");            // Print text
  }
  
  inkplate.display(); // Do a full refresh

  delay(60000); // Wait one minute
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint8_t _month,
               uint16_t _year)
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