/*
    Inkplate_RTC example for soldered.com Inkplate 2
    For this example you will need USB cable and the Inkplate 2.

    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how to set an alarm time and periodically check if it's been reached by getting the
    current time from a NTP server

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    28 Nov 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

// Adjust your time zone, 2 means UTC+2
int timeZone = 1;

#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Create network object for WiFi and HTTP functions
Network network;

// Initialize Inkplate object
Inkplate display;

// Write your SSID and password
char ssid[] = "";
char pass[] = "";

// Structure that contains time info
struct tm currentTime, timerTime;

// Set how many minutes should pass between each timer check
int minutesBetweenWakes = 20;

// Set the time and date to trigger the alarm function
// Here it's set to 25.12. 8:00 AM, Christmas morning of the current year
struct alarmTime
{
    int hour = 00;
    int mins = 00;
    int day = 25;
    int mon = 12;
} alarmTime;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin();

    // Get the current time from the NTP servers
    // Note: WiFi must be connected
    network.getTime(&currentTime);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

    display.setTextColor(INKPLATE2_BLACK);

    // Copy currentTime nad set the correct hours, mins and date
    timerTime = currentTime;
    timerTime.tm_hour = alarmTime.hour;
    timerTime.tm_min = alarmTime.mins;
    timerTime.tm_sec = 0; // Start alarm at hour:mins:00s
    timerTime.tm_mday = alarmTime.day;
    timerTime.tm_mon = alarmTime.mon - 1; // Months are zero indexed

    // Calculate the difference between times
    int timeUntilAlarmInSeconds = difftime(mktime(&timerTime), mktime(&currentTime));

    // The alarm time has been reached!
    if (timeUntilAlarmInSeconds <= 0)
    {
        display.setTextSize(2);
        display.setCursor(9,30);
        display.printf("Merry Christmas!\n");
        display.setTextSize(1);
        display.printf("\n      It's %2.1d:%02d", currentTime.tm_hour, currentTime.tm_min);
        display.printf(" on %2.1d.%2.1d.%04d", currentTime.tm_mday, currentTime.tm_mon + 1, currentTime.tm_year + 1900);
        display.display(); // Show the data on the display

        // Do whatever the alarm should do here
        while(1)
        {
            delay(100);
        }
        
    }

    else
    {
        display.setTextSize(1);
        display.printf("\n           "); // For alignment
        display.printf("Waiting for: ");
        display.setTextSize(4);
        display.setCursor(0, 33);
        display.printf("  "); // For alignment
        display.printf("%2.1d:%02d", timerTime.tm_hour, timerTime.tm_min);
        display.setCursor(0, 65);
        display.setTextSize(2);
        display.printf("  "); // For alignment
        display.printf("on %2.1d.%2.1d.%04d", timerTime.tm_mday, timerTime.tm_mon + 1, timerTime.tm_year + 1900);

        display.display(); // Show the data on the display

        // Set how much the internal RTC should wait before waking the device up
        // It's in uS, so to get minutes we need to * 1000000 * 60
        // Note: The device still has to be powered during sleep time to wake up
        esp_sleep_enable_timer_wakeup(1000000 * 60 * minutesBetweenWakes);

        // Start sleep, this function never returns
        esp_deep_sleep_start();
    }
}

void loop()
{
}
