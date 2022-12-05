/*
    Inkplate_RTC_Periodic_Wake example for soldered.com Inkplate 2
    For this example you will need USB cable and the Inkplate 2.

    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will get and show the current time and date from the internet one time when Inkplate is turend on.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    1 Dec 2022 by Soldered
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
struct tm currentTime;

// Set how many minutes should pass between each wakeup
int minutesBetweenWakes = 5;

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

    // Set the cursor in the correct position
    display.setCursor(0, 10);

    // Print hours and minutes in text size 7
    // %2.1d adds a space in front of single digit integers
    // %02d adds a zero in front of single digit numbers
    display.setTextSize(7);
    display.printf("%2.1d:%02d\n", currentTime.tm_hour, currentTime.tm_min);

    // Print date in text size 3
    // Month has to be incremented by 1 because it's zero-indexed (January is 0, February 1 and so on)
    // Year has to be incremented by 1900
    display.setTextSize(3);
    display.printf(" %2.1d.%2.1d.%04d\n", currentTime.tm_mday, currentTime.tm_mon + 1, currentTime.tm_year + 1900);

    // Print how many minutes was set for the internal RTC
    display.setTextSize(1);
    display.printf("         See you in %d minutes!",minutesBetweenWakes);

    // Refresh the display with new data
    display.display();

    // Set how much the internal RTC should wait before waking the device up
    // It's in uS, so to get minutes we need to * 1000000 * 60 
    // Note: The device still has to be powered during sleep time to wake up
    esp_sleep_enable_timer_wakeup(1000000 * 60 * minutesBetweenWakes);

    // Start sleep, this function never returns
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}
