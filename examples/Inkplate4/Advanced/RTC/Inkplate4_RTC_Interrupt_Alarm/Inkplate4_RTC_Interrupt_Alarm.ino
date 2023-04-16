/*
   Inkplate4_RTC_Interrupt_Alarm example for Soldered Inkplate 4
   For this example you will need a USB-C cable and Inkplate 4.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   In this example we will show how to use PCF85063A RTC Alarm functionality with interrupt.
   This example will show how to set time and date, how to set up a alarm, how to read time, how to print time on
   Inkplate, and how to handle interrupt.

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

volatile int _alarmFlag = 0; // Variable to store alarm flag

void IRAM_ATTR alarmISR() // This function will be called when alarm interrupt event happens
{                         // NOTE: Function must be above setup() and loop()!
    _alarmFlag = 1;       // Set alarm flag
}

void setup()
{
    pinMode(39, INPUT_PULLUP); // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(2); // Set text to be 2 times bigger than classic 5x7 px text

    // Set time as an epoch
    display.rtcSetEpoch(1680676200);
    display.rtcSetAlarmEpoch(display.rtcGetEpoch() + 15, RTC_ALARM_MATCH_DHHMMSS);

    // Or you can use other way to set the time and date
    // display.rtcSetTime(6, 30, 0);
    // display.rtcSetDate(4, 5, 4, 2023);
    // display.rtcSetAlarm(15, 30, 6, 5, 4); // Set alarm 15 seconds from now

    attachInterrupt(39, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

void loop()
{
    display.clearDisplay();     // Clear frame buffer of display
    display.setCursor(20, 140); // Set position of the text
    display.rtcGetRtcData();    // Get the time and date from RTC

    // Print the time on screen
    printTime(display.rtcGetHour(), display.rtcGetMinute(), display.rtcGetSecond(), display.rtcGetDay(),
              display.rtcGetWeekday(), display.rtcGetMonth(), display.rtcGetYear());

    if (_alarmFlag) // Check alarm flag
    {
        // _alarmFlag = 0;              // Uncomment if you want to clear this flag
        display.rtcClearAlarmFlag(); // It's recommended to clear alarm flag after alarm has occurred
        display.setCursor(165, 160); // Set position of the text
        display.print("ALARM");      // Print text

        // Do whatever alarm should do here
    }

    // Show time on the screen
    display.display();

    delay(10000); // Delay between refreshes.
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

// A function that prints 2 digits
// It adds 0 before the number if it's only one digit
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
