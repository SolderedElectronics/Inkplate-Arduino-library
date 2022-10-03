/*
   Inkplate_RTC_Alarm_Deep_Sleep_Example example for Soldered Inkplate 6PLUS
   For this example you will need only USB cable and Inkplate 6PLUS
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/
   
   This example will show you how to use RTC alarm interrupt with deep sleep.
   Inkplate features RTC chip with interrupt for alarm connected to GPIO39
   Inkplate board will wake up every 10 seconds, refresh screen and go back to sleep.
   
   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 November 2021 by Soldered
*/

#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select Inkplate 6PLUS or Inkplate 6PLUS V2 in the boards menu."
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
  display.begin();              // Init Inkplate library (you should call this function ONLY ONCE)

  display.rtcClearAlarmFlag();  // Clear alarm flag from any previous alarm
  
  if (!display.rtcIsSet())      // Check if RTC is already is set. If ts not, set time and date
  {
    //  setTime(hour, minute, sec);
    display.rtcSetTime(6, 54, 00); // 24H mode, ex. 6:54:00
    //  setDate(weekday, day, month, yr);
    display.rtcSetDate(6, 16, 5, 2020); // 0 for Sunday, ex. Saturday, 16.5.2020.

    // display.rtcSetEpoch(1589610300); // Or use epoch for setting the time and date
  }

  printCurrentTime();   // Display current time and date
  display.display();
  
  display.rtcSetAlarmEpoch(display.rtcGetEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);   // Set RTC alarm 10 seconds from now

  display.frontlight(false);    // Disable frontlight (to save power)

  // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

  // Go to sleep
  esp_deep_sleep_start();
}

void loop()
{
  // Nothing...
}

void printCurrentTime()
{
  display.setCursor(100, 300);
  display.setTextSize(3);

  switch (display.rtcGetWeekday())
  {
    case 0:
      display.print("Sunday , ");
      break;
    case 1:
      display.print("Monday , ");
      break;
    case 2:
      display.print("Tuesday , ");
      break;
    case 3:
      display.print("Wednesday , ");
      break;
    case 4:
      display.print("Thursday , ");
      break;
    case 5:
      display.print("Friday , ");
      break;
    case 6:
      display.print("Saturday , ");
      break;
  }

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

void print2Digits(uint8_t _d)
{
  if (_d < 10)
    display.print('0');
  display.print(_d, DEC);
}
