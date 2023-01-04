/*
    Inkplate_RTC_Alarm_Sleep example for soldered.com Inkplate 2
    For this example you will need USB cable and the Inkplate 2.

    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how to use Inkplate's internal RTC to set an alarm, go to sleep and wake up at a desired
    time.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    28 Nov 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library

// Our networking functions, declared in Network.cpp
#include "Network.h"

Inkplate display; // Initialize Inkplate object

Network network; // Create network object for WiFi and HTTP functions

// Write your SSID and password (needed to get the correct time from the Internet)
char ssid[] = "";
char pass[] = "";

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Set the time to wake up (24 hour format)
// Here it's set to 7:30
// Give it at least a few minutes of space before activating for the code to compile and upload to Inkplate
// If you want to sound the alarm at a different date, uncomment day and month here and in setup()
// Note: it takes approx 25 seconds to wake up from sleep and display the time
struct alarmTime
{
    int hour = 7;
    int mins = 30;
    int day = 5;
    int mon = 1;
} alarmTime;

// Structure that contains time info
struct tm currentTime, timerTime;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin(ssid, pass);

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
        network.getTime(&currentTime, timeZone);

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

        // Set timer time's hours and minutes for when to wake up
        // If you need to have the alarm on a different date, set it here
        timerTime = currentTime;
        timerTime.tm_hour = alarmTime.hour;
        timerTime.tm_min = alarmTime.mins;
        timerTime.tm_sec = 0; // Start alarm at hour:mins:00
        timerTime.tm_mday = alarmTime.day;        
        timerTime.tm_mon = alarmTime.mon -1 ; // Months are zero indexed

        // Calculate seconds until alarm
        long timeUntilAlarmInSeconds = difftime(mktime(&timerTime), mktime(&currentTime));

        if (timeUntilAlarmInSeconds <= 0)
        {
            // Set alarm time can't be in the past!
            // Check variable alarmTime
            display.setTextSize(1);
            display.printf("\n Error: Set alarm time\n (%2.1d:%02d) is earlier than or same as now!", timerTime.tm_hour,
                           timerTime.tm_min);
            display.display();

            // Do whatever the alarm should do here
            while (1)
            {
                delay(100);
            }
        }
        else
        {
            // Print info about currently set alarm
            display.setTextSize(1);
            display.println();
            display.print("  "); // Print spaces for alignment
            display.print("Alarm is set for ");
            display.printf("%2.1d:%02d", timerTime.tm_hour, timerTime.tm_min);
            display.print(" on ");            
            display.printf("%2.1d.%02d\n", timerTime.tm_mday, timerTime.tm_mon + 1);
            display.print("       "); // Print spaces for alignment
            display.print("Going to sleep, bye!");
            display.display(); // Show everything on the display
            delay(100);

            // Go to sleep until it's time for the alarm
            // It's in uS, so * 1000000 to convert from seconds
            // Note: The device still has to be powered during sleep time to wake up
            esp_sleep_enable_timer_wakeup(timeUntilAlarmInSeconds * 1000000LL);

            // Start deep sleep (this function does not return)
            esp_deep_sleep_start();
        }
    }
    else
    {
        // This code runs if the device has woken up from sleep and the alarm time has been reached

        display.setTextColor(INKPLATE2_BLACK);
        display.setCursor(43, 25); // Set cursor in correct position to print alarm text
        display.setTextSize(4);

        display.println("ALARM");
        display.setTextSize(2);
        display.printf("   It's %02d:%02d!\n", alarmTime.hour, alarmTime.mins); // Print set alarm time

        display.display(); // Show everything on the display
    }
}

void loop()
{
    // Nothing. Loop must be empty!
}
