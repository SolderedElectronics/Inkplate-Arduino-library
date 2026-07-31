/**
 **************************************************
 * @file        Inkplate2_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       RTC alarm emulation with deep sleep: fetch time via NTP, set an
 *              RTC match alarm, sleep, then wake and show "ALARM".
 *
 * @details     This example demonstrates how to combine network time (NTP),
 *              the Inkplate RTC helper, and ESP32 deep sleep to wake the board
 *              at a desired time. On first boot (non-timer wake), the sketch:
 *              connects to WiFi, obtains the current time from the Internet,
 *              prints the current time/date on the e-paper, then configures an
 *              RTC "alarm" using a selected match mode (e.g. minutes+seconds).
 *
 *              The RTC alarm here is an emulation implemented in software: the
 *              RTC configuration is converted to a number of seconds until the
 *              next match, and the ESP32 is put into deep sleep until that
 *              moment. When the device wakes due to the sleep timer, it prints
 *              "ALARM" on the display.
 *
 *              Because deep sleep resets the ESP32 on every wake, execution
 *              always starts from setup(), so keep logic there and leave loop()
 *              empty. The wake reason is checked with
 *              esp_sleep_get_wakeup_cause() to distinguish initial boot from an
 *              alarm wake.
 *
 *              Display mode is 1-bit (BW) and this example uses a full refresh
 *              (display()). The RTC "alarm" depends on the selected match mode:
 *              if you match only minutes and seconds (RTC_MMSS), the chosen
 *              values must represent a future match relative to the current
 *              time, otherwise the alarm cannot be scheduled - for alarms on a
 *              later day, include day (and possibly month) in the match. The
 *              example relies on Internet time for initial RTC setup, so without
 *              WiFi the current time cannot be obtained.
 *
 *              Expected output: on first boot, the current time/date plus a
 *              message indicating seconds until the alarm (or an error if the
 *              alarm time is not valid), then deep sleep; on alarm wake, large
 *              "ALARM" text on the e-paper display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi network (SSID/password), Internet access for NTP
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password (ssid, pass) and your local timezone offset
 *    (timeZone, hours from UTC).
 * 3) Set the desired alarmTime fields and select an RTC match mode
 *    (e.g. RTC_MMSS).
 * 4) Upload the sketch and open Serial Monitor at 115200 baud (optional).
 * 5) On first run, the display shows current time/date and the configured
 *    seconds until the alarm, then the board enters deep sleep.
 * 6) When the alarm time is reached, the board wakes and shows "ALARM".
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-11-28
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library

#include "NetworkFunctions.h" // Our networking functions, declared in Network.cpp

#include "RTC.h" // Our RTC functions, declared in RTC.cpp

Inkplate display; // Initialize Inkplate object

NetworkFunctions network; // Create network object for WiFi and HTTP functions

RTC rtc; // RTC object for RTC functions

// Write your SSID and password (needed to get the correct time from the Internet)
char ssid[] = "";
char pass[] = "";

// Adjust your time zone, 1 means UTC+1
int timeZone = 1;

// Setting the alarm starts at line 96

// Structure that contains time info
struct tm currentTime, timerTime, alarmTime;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin(ssid, pass);
    network.setTime(timeZone);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

    // Check why the device woke up
    // If it woke up because of the internal RTC's timer, esp_sleep_get_wakeup_cause will be ESP_SLEEP_WAKEUP_TIMER
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER)
    {
        // This code runs initially, to get current time and set the timer

        display.setTextColor(INKPLATE2_BLACK);
        display.setCursor(40, 20); // Set cursor in correct position to print hours
        display.setTextSize(4);

        // Get the current time from the NTP server and store it in the RTC
        network.getTime(&currentTime);

        // Now it's possible to access the time data via the tm object

        // Print hours and minutes
        // %2.1d adds a space in front of single digit integers
        // %02d adds a zero in front of single digit numbers
        display.printf("%2.1d:%02d\n", currentTime.tm_hour, currentTime.tm_min);

        // Print date
        // Month has to be incremented by 1 because it's zero-indexed (January is 0, February 1 and so on)
        // Year has to be incremented by 1900
        display.setTextSize(2);
        display.print("   "); // Print spaces for alignment
        display.printf("%2.1d.%2.1d.%04d\n", currentTime.tm_mday, currentTime.tm_mon + 1, currentTime.tm_year + 1900);

        // Set time zone
        rtc.setTimezone(timeZone);

        // Set alarm - here is set to 5.1. at 09:30:00
        // Give it at least a few minutes of space before activating for the code to compile and upload to Inkplate
        // Note: it takes approx 25 seconds to wake up from sleep and display the time
        alarmTime.tm_hour = 9;
        alarmTime.tm_min = 30;
        alarmTime.tm_sec = 0;
        alarmTime.tm_mday = 5;
        alarmTime.tm_mon = 1;

        // Note: alarm depends on which RTC match you use. In this case,
        // it will compare only minutes and seconds. All others alarm parameters will be ignored.
        double secondsUntilAlarm = rtc.setAlarm(alarmTime, RTC_MMSS);

        // Or you can set an alarm with time epoch
        // double secondsUntilAlarm = rtc.setAlarmEpoch(1675854420, RTC_MMSS);

        if (secondsUntilAlarm > 0)
        {
            // Print info about currently set alarm
            display.setTextSize(1);
            display.println();
            display.printf("The alarm is set %ld seconds from now. Going to sleep, bye!", (long)secondsUntilAlarm);
            display.display(); // Show everything on the display
            delay(100);

            // Go to deep sleep
            deepSleep();
        }
        else
        {
            // Set alarm time can't be in the past!
            // Check variable alarmTime

            // The screen will display an error and go to the deep sleep
            display.setTextSize(1);
            display.printf("\nError: Set alarm time is earlier \nthan or same as now! Change time or RTC match");
            display.display();

            // Go to deep sleep
            deepSleep();
        }
    }
    else
    {
        // This code runs if the device has woken up from sleep and the alarm time has been reached

        display.setTextColor(INKPLATE2_BLACK);
        display.setCursor(43, 25); // Set cursor in correct position to print alarm text
        display.setTextSize(4);
        display.println("ALARM");
        display.display(); // Show everything on the display

        // Do whatever alarm should do here

        // After the alarm, you can put Inkplate in deep sleep (calling deepSleep() function defined below) or set a
        // timer for another alarm
    }
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void deepSleep()
{
    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}