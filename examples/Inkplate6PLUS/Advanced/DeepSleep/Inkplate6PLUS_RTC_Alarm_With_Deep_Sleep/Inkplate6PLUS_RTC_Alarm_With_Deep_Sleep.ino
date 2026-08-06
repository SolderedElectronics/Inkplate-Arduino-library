/**
 **************************************************
 * @file        Inkplate6PLUS_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       RTC alarm wake-up with deep sleep for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to use the onboard RTC alarm interrupt to wake
 *              the Inkplate 6PLUS from ESP32 deep sleep. The RTC alarm is configured
 *              to trigger periodically, waking the board, refreshing the
 *              e-paper display with the current date and time, and then
 *              returning the system back to low-power deep sleep mode.
 *
 *              The RTC alarm interrupt is connected to GPIO39 on Inkplate 6PLUS.
 *              When using deep sleep, all application logic must be placed in
 *              setup() and loop() must remain empty. Expected output is the
 *              current weekday, date and time on the Inkplate display,
 *              refreshed automatically on every RTC alarm wake-up.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      None
 * - Serial:     115200 baud (optional, not required for output)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) On first boot, the RTC time and date are initialized if not already set,
 *    and the current time and date are shown on the display.
 * 4) The board enters deep sleep and wakes up every 10 seconds using the RTC
 *    alarm.
 * 5) After each wake-up, the display is refreshed and the board goes back to
 *    sleep.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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

    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS); // Set RTC alarm 10 seconds from now

    display.frontlight.setState(false); // Disable frontlight (to save power)

    // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void printCurrentTime()
{
    display.setCursor(100, 300);
    display.setTextSize(3);

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
