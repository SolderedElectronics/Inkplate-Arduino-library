/*
   Inkplate_RTC_Interrupt_Alarm_Example example for SolderedInkplate 5
   For this example you will need USB cable and Inkplate 5.
   Select "Inkplate 5(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 5(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   In this example we will show how to use PCF85063A RTC Alarm functionality with interrupt.
   This example will show how to set time and date, how to set up a alarm, how to read time, how to print time on Inkplate using partial updates and how to handle interrupt.
   NOTE: Partial update is only available on 1 Bit mode (BW) and it is not recommended to use it on first refresh after
   power up. It is recommended to do a full refresh every 5-10 partial refresh to maintain good picture quality.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   12 November 2021 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Inkplate 5 in the boards menu."
#endif

#include "Inkplate.h"             // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT);  // Create an object on Inkplate library and also set library into 1-bit mode (BW)

volatile int _alarmFlag = 0;      // Variable to store alarm flag

void IRAM_ATTR alarmISR()         // This function will be called when alarm interrupt event happens
{                                 // NOTE: Function must be above setup() and loop()!
  _alarmFlag = 1;                 // Set alarm flag
}

void setup()
{
    pinMode(39, INPUT_PULLUP);      // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(4); // Set text to be 4 times bigger than classic 5x7 px text
  
    display.rtcSetEpoch(1589610300);
    display.rtcSetAlarmEpoch(display.rtcGetEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // display.rtcSetTime(6, 25, 0);        // Or you can use other way to set the time and date
    // display.rtcSetDate(6, 16, 5, 2020);
    // display.rtcSetAlarm(10, 25, 6, 16, 6); // Set alarm 10 seconds from now
  
    attachInterrupt(39, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

// Variable that keeps count on how much screen has been partially updated
int n = 0;
void loop()
{
    display.clearDisplay();         // Clear frame buffer of display
    display.setCursor(100, 100);    // Set position of the text
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

    delay(700);                             // Delay between refreshes.
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
