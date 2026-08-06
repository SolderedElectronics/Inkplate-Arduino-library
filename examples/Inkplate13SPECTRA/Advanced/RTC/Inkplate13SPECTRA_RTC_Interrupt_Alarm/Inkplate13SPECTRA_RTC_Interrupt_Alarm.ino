/**
 **************************************************
 * @file        Inkplate13SPECTRA_RTC_Interrupt_Alarm.ino
 * @brief       RTC alarm with interrupt example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to use the PCF85063A RTC alarm functionality with an
 *              interrupt: how to set time and date, how to set up an alarm, how
 *              to read the time, how to print the time on the Inkplate display
 *              and how to handle the interrupt.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The sketch sets the time and date and arms the alarm with its interrupt.
 * 4) When the alarm fires, the interrupt is handled and the time is printed on
 *    the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-02
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
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
  inkplate.setTextSize(4);    // Set text to be 4 times bigger than classic 5x7 px text

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