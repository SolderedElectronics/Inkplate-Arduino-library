/*
    Inkplate7_RTC_Alarm example for Soldered Inkplate 7
    For this example you will need USB cable and the Inkplate 7.

    Select "Soldered Inkplate7" from Tools -> Board menu.
    Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will get and show the current time and date from the internet.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    3 Jul 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

#include "Inkplate.h" // Inlcude Inkplate library

#include "Network.h" // Our networking functions, declared in Network.cpp

Inkplate display; // Initialize Inkplate object

Network network; // Create network object for WiFi and HTTP functions

// Write your SSID and password (needed to get the correct time from the Internet)
char ssid[] = "";
char pass[] = "";

// Define a delay time of 40 seconds. It's around a minute if we add the refresh display time
#define DELAY_TIME 40 * 1000

// Structure that contains time info
struct tm currentTime;

// This sketch fetches time data from a NTP server via WiFi and printing it.

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin(ssid, pass);

    // Init library (you should call this function ONLY ONCE)
    display.begin();

    // Set text color to black
    display.setTextColor(INKPLATE7_BLACK); 
}

void loop()
{
    // Get the current time from the NTP servers
    // Note: WiFi must be connected
    network.getTime(&currentTime, timeZone);

    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

    // Set the cursor in the correct position
    display.setCursor(145, 120);

    // Print hours and minutes in text size 13
    // %2.1d adds a space in front of single digit integers
    // %02d adds a zero in front of single digit numbers
    display.setTextSize(13);
    display.printf("%2.1d:%02d\n", currentTime.tm_hour, currentTime.tm_min);

    // Print date in text size 7
    // Month has to be incremented by 1 because it's zero-indexed (January is 0, February 1 and so on)
    // Year has to be incremented by 1900
    display.setTextSize(7);
    display.setCursor(60, 230);
    display.printf(" %2.1d.%2.1d.%04d.\n", currentTime.tm_mday, currentTime.tm_mon + 1, currentTime.tm_year + 1900);

    // Refresh the display with new data
    display.display();

    // Delay before the next printing
    delay(DELAY_TIME);
}
