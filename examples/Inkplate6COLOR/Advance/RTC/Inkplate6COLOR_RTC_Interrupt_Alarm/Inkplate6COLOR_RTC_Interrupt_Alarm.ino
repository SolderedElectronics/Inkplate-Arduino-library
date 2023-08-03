/*
   Inkplate6COLOR_RTC_Interrupt_Alarm example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   In this example we will show how to use PCF85063A RTC Alarm functionality with interrupt.
   This example will show how to set time and date, how to set up a alarm, how to read time, how to print time on Inkplate and how to handle interrupt.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   19 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"             // Include Inkplate library to the sketch
Inkplate display;  // Create an object on Inkplate library and also set library into 1-bit mode (BW)

volatile int _alarmFlag = 0;      // Variable to store alarm flag

void IRAM_ATTR alarmISR()         // This function will be called when alarm interrupt event happens
{                                 // NOTE: Function must be above setup() and loop()!
  _alarmFlag = 1;                 // Set alarm flag
}

void setup()
{
    pinMode(39, INPUT_PULLUP);      // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtcReset();           //  Reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
  
    display.rtcSetEpoch(1650000000);
    display.rtcSetAlarmEpoch(display.rtcGetEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS);

    // display.rtcSetTime(6, 25, 0);        // Or you can use other way to set the time and date
    // display.rtcSetDate(6, 16, 5, 2020);
    // display.rtcSetAlarm(10, 25, 6, 16, 6); // Set alarm 10 seconds from now
  
    attachInterrupt(39, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

void loop()
{
    display.clearDisplay();         // Clear frame buffer of display
    display.setCursor(60, 100);    // Set position of the text
    display.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         //Set text color and background
    display.rtcGetRtcData();          // Get the time and date from RTC

    // Print the time on screen
    printTime(display.rtcGetHour(), display.rtcGetMinute(), display.rtcGetSecond(), display.rtcGetDay(), display.rtcGetWeekday(), display.rtcGetMonth(), display.rtcGetYear());
    
    if (_alarmFlag)     // Check alarm flag
    {
        // _alarmFlag = 0;              // Uncomment if you want to clear this flag
        display.rtcClearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
        display.setCursor(200, 200);    // Set position of the text
        display.print("ALARM");         // Print text
    }
    
     display.display(); // Do a full refresh

     delay(60000); //Wait one minute
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
