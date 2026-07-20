/*
  Inkplate7SPECTRA_RTC_Alarm_With_Deep_Sleep example for Soldered Inkplate 7SPECTRA
  For this example you will need only USB cable and Inkplate 7SPECTRA
  Select "Soldered Inkplate 7SPECTRA" from Tools ->Board menu.
  Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
  https://docs.soldered.com/inkplate/7spectra/quick-start-guide/

  This example will show you how to use RTC alarm interrupt with deep sleep.
  Inkplate features RTC chip with interrupt for alarm connected to GPIO18
  Inkplate board will wake up every 60 seconds, refresh screen and go back to sleep.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  Looking to get support? Write on our forums: https://forum.soldered.com/
  21 January 2025 by Soldered
*/

#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

#include "Inkplate.h" //Include inkplate library to the sketch
#include "driver/rtc_io.h"  //Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h> //Include ESP32 library for RTF (needed for rtc_get_reset_reason() function)

Inkplate inkplate; //Create an object on Inkplate library

void setup() {
  inkplate.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.rtc.clearAlarmFlag(); // Clear alarm flag from any previous alarm
  if(!inkplate.rtc.isSet()) // Check if RTC is already is set. If ts not, set time and date
  {  
    //  display.setTime(hour, minute, sec);
    inkplate.rtc.setTime(13,30,00); // 24H mode, ex. 13:30:00
    //  display.setDate(weekday, day, month, yr);
    inkplate.rtc.setDate(1,20,7,2026); // 0 for Monday, ex. Saturday, 5.12.2022.

    // display.rtc.setEpoch(1589610300); // Or use epoch for setting the time and date
  }
  printCurrentTime(); // Display current time and date
  inkplate.display();

  inkplate.rtc.setAlarmEpoch(inkplate.rtc.getEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS); // Set RTC alarm 60 seconds from now

  // Enable wakup from deep sleep on gpio 2 where RTC interrupt is connected
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 0);

  // Start deep sleep (this function does not return). Program stops here.
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void printCurrentTime(){
  inkplate.setCursor(50,250);
  inkplate.setTextSize(2);
  inkplate.setTextColor(INKPLATE_BLUE, INKPLATE_WHITE); // Set text color and background

  inkplate.rtc.getRtcData();
  switch (inkplate.rtc.getWeekday())
    {
    case 0:
        inkplate.print("Sunday , ");
        break;
    case 1:
        inkplate.print("Monday , ");
        break;
    case 2:
        inkplate.print("Tuesday , ");
        break;
    case 3:
        inkplate.print("Wednesday , ");
        break;
    case 4:
        inkplate.print("Thursday , ");
        break;
    case 5:
        inkplate.print("Friday , ");
        break;
    case 6:
        inkplate.print("Saturday , ");
        break;
    }
    inkplate.print(inkplate.rtc.getDay());
    inkplate.print(".");
    inkplate.print(inkplate.rtc.getMonth());
    inkplate.print(".");
    inkplate.print(inkplate.rtc.getYear());
    inkplate.print(". ");
    print2Digits(inkplate.rtc.getHour());
    inkplate.print(':');
    print2Digits(inkplate.rtc.getMinute());
    inkplate.print(':');
    print2Digits(inkplate.rtc.getSecond());
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        inkplate.print('0');
    inkplate.print(_d, DEC);
}