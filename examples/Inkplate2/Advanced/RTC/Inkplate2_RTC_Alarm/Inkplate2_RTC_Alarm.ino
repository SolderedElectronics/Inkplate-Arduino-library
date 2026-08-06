/**
 **************************************************
 * @file        Inkplate2_RTC_Alarm.ino
 * @brief       Display current time and date fetched from the Internet (NTP)
 *              and refresh periodically.
 *
 * @details     This example demonstrates how to obtain the current time from an
 *              NTP server over WiFi using the provided Network helper and then
 *              show the time and date on the Inkplate 2 e-paper display.
 *
 *              The sketch connects to WiFi, requests time data into a standard
 *              tm structure, formats hours/minutes and the calendar date, and
 *              performs a full display refresh. The process repeats at a fixed
 *              interval (DELAY_TIME). Despite the filename, this sketch does
 *              not configure an RTC alarm or deep sleep; it only periodically
 *              updates the displayed time.
 *
 *              Display mode is 1-bit (BW) and the sketch uses a full refresh
 *              (display()). Frequent full refreshes increase update time and may
 *              cause more visible flashing, so consider longer intervals for
 *              battery-powered use. WiFi must remain connected for each NTP
 *              fetch; if WiFi is unavailable, time retrieval will fail (add
 *              error handling for production use).
 *
 *              Expected output: a large HH:MM time with the DD.MM.YYYY date
 *              below it on the display; the Serial Monitor only shows basic logs
 *              if you add your own prints.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access (NTP)
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password (ssid, pass) and set your local timeZone
 *    (hours offset from UTC) and DELAY_TIME (ms) update period.
 * 3) Upload the sketch to Inkplate 2.
 * 4) The display shows the current time and date.
 * 5) The screen is refreshed again after each DELAY_TIME interval.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-01
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

#include "Inkplate.h" // Inlcude Inkplate library

#include "NetworkFunctions.h" // Our networking functions, declared in Network.cpp

Inkplate display; // Initialize Inkplate object

NetworkFunctions network; // Create network object for WiFi and HTTP functions

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
    display.setTextColor(INKPLATE2_BLACK); 
}

void loop()
{
    // Get the current time from the NTP servers
    // Note: WiFi must be connected
    network.getTime(&currentTime, timeZone);

    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

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

    // Refresh the display with new data
    display.display();

    // Delay before the next printing
    delay(DELAY_TIME);
}