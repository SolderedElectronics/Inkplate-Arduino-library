/*
    Inkplate2_RTC_Alarm_With_Deep_Sleep example for Soldered Inkplate 2
    For this example you will need USB cable and the Inkplate 2.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how to use Inkplate's internal RTC to set an alarm, go to sleep and wake up at a desired
    time.

    NOTE: This is not a real alarm on the RTC, just an emulation. If you choose to match minutes and seconds,
    they must be greater than the current time, otherwise the alarm will not be set. If it is necessary,
    for example, to set an alarm for tomorrow morning, and now it is noon, it is not possible to set only
    hours and minutes and set a match to them, because that time is earlier than the current time if we only
    look at hours and minutes. You can set such an alarm by setting the match to days, hours and minutes and
    then it will work ok, you just have to set the day as well. Likewise for other cases

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    28 Nov 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library

#include "Network.h" // Our networking functions, declared in Network.cpp

#include "RTC.h" // Our RTC functions, declared in RTC.cpp

Inkplate display; // Initialize Inkplate object

Network network; // Create network object for WiFi and HTTP functions

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
    // Put the panel in the deep sleep
    display.setPanelDeepSleep(0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}
