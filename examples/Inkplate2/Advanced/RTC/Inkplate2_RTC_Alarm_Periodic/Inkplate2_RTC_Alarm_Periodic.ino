/*
    Inkplate2_RTC_Alarm_Periodic for soldered.com Inkplate 2
    For this example you will need USB cable and the Inkplate 2.

    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how to set an alarm time and periodically check if it's been reached by getting the
    current time from a NTP server

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    28 Nov 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library

#include "Network.h" // Our networking functions, declared in Network.cpp

#include "RTC.h" // Our RTC functions, declared in RTC.cpp

Inkplate display; // Initialize Inkplate object

Network network; // Create network object for WiFi and HTTP functions

RTC rtc; // Create RTC object for RTC functions

// Write your SSID and password (needed to get the correct time from the Internet)
char ssid[] = "";
char pass[] = "";

// Adjust your time zone, 1 means UTC+1
int timeZone = 1;

// Set the time and date to trigger the alarm function
// Here it's set to 25.12. 8:00:00 AM, Christmas morning of the current year
int alarmHour = 8;
int alarmMins = 0;
int alarmSecs = 0;
int alarmDay = 25;
int alarmMon = 12;

// Structure that contains time info
struct tm currentTime;

// Set the period to wake up - every 1 hour and 30 minutes wake up and check if it is time for the alarm
int wakeHours = 1;
int wakeMinutes = 30;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin(ssid, pass);

    // Get the current time from the NTP servers
    // Note: WiFi must be connected
    network.getTime(&currentTime, timeZone);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

    display.setTextColor(INKPLATE2_BLACK);

    int timeUntilAlarmInSeconds = rtc.secondsUntilAlarm(alarmHour, alarmMins, alarmSecs, alarmDay, alarmMon, currentTime);

    // The alarm time has been reached!
    if (timeUntilAlarmInSeconds <= 0)
    {
        // Display the screen when the alarm is reached
        alarmScreen();

        // Do whatever the alarm should do here
        while (1)
        {
            delay(100);
        }
    }
    else
    {
        // Display the screen for waiting for the alarm
        waitingScreen();

        rtc.setWakeUpTimer(wakeHours, wakeMinutes, currentTime);

        // Start sleep, this function never returns, program stops here
        esp_deep_sleep_start();
    }
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void waitingScreen()
{
    display.setTextSize(1);
    display.printf("\n           "); // For alignment
    display.printf("Waiting for: ");
    display.setTextSize(4);
    display.setCursor(0, 33);
    display.printf("  "); // For alignment
    display.printf("%2.1d:%02d", alarmHour, alarmMins);
    display.setCursor(0, 65);
    display.setTextSize(2);
    display.printf("    "); // For alignment
    display.printf("on %2.1d.%2.1d.", alarmDay, alarmMon);

    display.display(); // Show the data on the display
}

void alarmScreen()
{
    display.setTextSize(2);
    display.setCursor(9, 30);
    display.printf("ALARM!\n");
    display.setTextSize(1);

    display.display(); // Show the data on the display
}
