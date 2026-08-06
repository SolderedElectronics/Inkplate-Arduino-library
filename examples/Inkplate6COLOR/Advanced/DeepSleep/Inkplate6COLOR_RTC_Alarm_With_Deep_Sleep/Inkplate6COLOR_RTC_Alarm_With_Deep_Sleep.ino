/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       Uses the RTC alarm interrupt to wake Inkplate 6COLOR from
 *              deep sleep at fixed intervals.
 *
 * @details     This example demonstrates how to use the onboard RTC on
 *              Inkplate 6COLOR to schedule a timed wake-up every 60 seconds.
 *              On each boot, the sketch initializes the display, checks
 *              whether the RTC time/date is already set, prints the current
 *              date and time on the e-paper display, programs the next RTC
 *              alarm, and then enters deep sleep.
 *
 *              The RTC alarm output is connected to GPIO39 and is used as the
 *              wake source. Because ESP32 deep sleep resets the MCU, the full
 *              application flow must be placed inside setup() and loop() must
 *              remain empty. After each wake-up, the board starts execution
 *              again from the beginning of setup(), refreshes the display,
 *              schedules the next alarm, and returns to sleep. RTC alarm and
 *              wake-up are separate steps: the RTC generates the interrupt, and
 *              ESP32 deep sleep wake-up is enabled separately on GPIO39.
 *
 *              This example is useful for low-power clock, scheduler, and
 *              periodic refresh applications where the display only needs to
 *              update occasionally. It only sets the RTC time/date if the RTC is
 *              not already configured. Display mode is Inkplate 6COLOR color
 *              e-paper mode; frequent full colour refreshes consume more time and
 *              energy than monochrome partial-update workflows on supported
 *              boards.
 *
 *              Expected output: the current weekday, date and time rendered on
 *              the screen, with the board waking once per minute, refreshing the
 *              display and returning to deep sleep.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     not used in this example
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) If needed, adjust the initial RTC time/date values in the sketch before
 *    uploading.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) After boot, the current date and time are drawn on the display.
 * 5) The sketch sets an RTC alarm for 60 seconds in the future.
 * 6) Inkplate enters deep sleep and wakes again when the RTC alarm triggers
 *    on GPIO39.
 * 7) After wake-up, the ESP32 restarts, redraws the time, sets the next alarm,
 *    and repeats the cycle.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    display.rtc.clearAlarmFlag(); // Clear alarm flag from any previous alarm

    if (!display.rtc.isSet()) // Check if RTC is already is set. If ts not, set time and date
    {
        //  display.setTime(hour, minute, sec);
        display.rtc.setTime(13, 30, 00); // 24H mode, ex. 13:30:00
        //  display.setDate(weekday, day, month, yr);
        display.rtc.setDate(1, 5, 12, 2022); // 0 for Monday, ex. Saturday, 5.12.2022.

        // display.rtc.setEpoch(1589610300); // Or use epoch for setting the time and date
    }

    printCurrentTime(); // Display current time and date
    display.display();

    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS); // Set RTC alarm 60 seconds from now

    // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void printCurrentTime()
{
    display.setCursor(50, 250);
    display.setTextSize(3);
    display.setTextColor(INKPLATE_BLUE, INKPLATE_WHITE); // Set text color and background

    display.rtc.getRtcData();

    switch (display.rtc.getWeekday())
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

    display.print(display.rtc.getDay());
    display.print(".");
    display.print(display.rtc.getMonth());
    display.print(".");
    display.print(display.rtc.getYear());
    display.print(". ");
    print2Digits(display.rtc.getHour());
    display.print(':');
    print2Digits(display.rtc.getMinute());
    display.print(':');
    print2Digits(display.rtc.getSecond());
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
