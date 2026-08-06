/**
 **************************************************
 * @file        Inkplate5V2_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       RTC alarm wake-up with deep sleep for Soldered Inkplate 5v2.
 *
 * @details     Demonstrates how to use the onboard RTC alarm interrupt to wake
 *              the Inkplate 5v2 from ESP32 deep sleep. The RTC alarm is configured
 *              to trigger periodically, waking the board, refreshing the
 *              e-paper display with the current date and time, and then
 *              returning the system back to low-power deep sleep mode.
 *
 *              The RTC alarm interrupt is connected to GPIO39 on Inkplate 5v2.
 *              When using deep sleep, all application logic must be placed in
 *              setup() and loop() must remain empty. Expected output is the
 *              current weekday, date and time on the Inkplate display,
 *              refreshed automatically on every RTC alarm wake-up.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      None
 * - Serial:     115200 baud (optional, not required for output)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5v2.
 * 3) On first boot, the RTC time and date are initialized if not already set,
 *    and the current time and date are shown on the display.
 * 4) The board enters deep sleep and wakes up every 10 seconds using the RTC
 *    alarm.
 * 5) After each wake-up, the display is refreshed and the board goes back to
 *    sleep.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h" 

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT); 

void setup()
{
    display.begin();

    // Always clear the framebuffer first
    display.clearDisplay();

    // Force visible text: black on white
    display.setTextColor(BLACK, WHITE);

    display.rtc.clearAlarmFlag();

    if (!display.rtc.isSet())
    {
        display.rtc.setTime(6, 54, 0);
        display.rtc.setDate(2, 21, 3, 2023);
    }

    printCurrentTime();

    // Push framebuffer to the panel
    display.display();

    // Give it a moment (debug safety)
    delay(500);

    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // NOTE: GPIO39 is NOT guaranteed for Inkplate 5v2; this only affects wakeups,
    // not the first draw. Still, keep this for now.
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Print the current time on the screen
void printCurrentTime()
{
    // Set cursor and font size
    display.setCursor(385, 360);
    display.setTextSize(3);

    // Get data from the RTC
    display.rtc.getRtcData();

    // Find a weekday and print its full name
    switch (display.rtc.getWeekday())
    {
    case 0:
        display.print("Sunday, ");
        break;
    case 1:
        display.print("Monday, ");
        break;
    case 2:
        display.print("Tuesday, ");
        break;
    case 3:
        display.print("Wednesday, ");
        break;
    case 4:
        display.print("Thursday, ");
        break;
    case 5:
        display.print("Friday, ");
        break;
    case 6:
        display.print("Saturday, ");
        break;
    }

    // Print date and time
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

// A function that prints 2 digits
// It adds 0 before the number if it's only one digit
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
