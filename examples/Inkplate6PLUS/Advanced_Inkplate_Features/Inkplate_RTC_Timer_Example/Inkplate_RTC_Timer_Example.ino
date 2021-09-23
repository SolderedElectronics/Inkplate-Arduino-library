/*
   Inkplate_RTC_Timer_Example example for e-radionica Inkplate 6PLUS
   For this example you will need only USB cable and Inkplate 6PLUS
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows you timer usage of Inkplate RTC.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   19 August 2021 by e-radionica.com
*/

#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6PLUS in the boards menu."
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10      // Time how long ESP32 will be in deep sleep (in seconds). In this case 10 seconds.

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

int countdown_time = 10;

void setup()
{
    Serial.begin(115200);
    display.begin();

    display.rtcSetTime(6, 54, 00);      // 24H mode, ex. 6:54:00
    display.rtcSetDate(6, 16, 5, 2020); // 0 for Sunday, ex. Saturday, 16.5.2020.

    display.setCursor(0, 100);
    display.setTextSize(3);
    display.print("Now is:");
}

void loop()
{
    printCurrentTime();
    display.print("Setting timer countdown, waking up in ");
    display.print(countdown_time);
    display.println(" seconds.");
    // while(!display.available());

    /*   source_clock
     *       Inkplate::TIMER_CLOCK_4096HZ     -> clk = 4096Hz -> min timer = 244uS -> MAX timer = 62.256mS
     *       Inkplate::TIMER_CLOCK_64HZ       -> clk = 64Hz   -> min timer = 15.625mS -> MAX timer = 3.984s
     *       Inkplate::TIMER_CLOCK_1HZ        -> clk = 1Hz    -> min timer = 1s -> MAX timer = 255s
     *       Inkplate::TIMER_CLOCK_1PER60HZ   -> clk = 1/60Hz -> min timer = 60s -> MAX timer = 4h15min
     *   value
     *       coundowntime in seconds
     *   int_enable
     *       true = enable interrupt; false = disable interrupt
     *   int_pulse
     *       true = interrupt generate a pulse; false = interrupt follows timer flag
     */
    display.rtcTimerSet(Inkplate::TIMER_CLOCK_1HZ, countdown_time, false, false);

    display.print("Waiting for a countdown");
    while (!display.rtcCheckTimerFlag())
    {
        display.print(".");
        delay(1000);
    }

    display.print("\nInterrupt triggered on: ");
}

void printCurrentTime()
{
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
    display.print(display.rtcGetHour());
    display.print(":");
    display.print(display.rtcGetMinute());
    display.print(":");
    display.println(display.rtcGetSecond());

    display.display();
}
