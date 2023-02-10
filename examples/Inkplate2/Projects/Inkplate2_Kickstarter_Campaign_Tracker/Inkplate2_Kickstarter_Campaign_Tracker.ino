/*
    Inkplate2_Kickstarter_Campaign_Tracker example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 2 to track a campaign on Kickstarter.
    We find this API for our Kickstarter campaign using Google Chrome and inspecting network data.
    The page periodically receives stats.json?v=1 file and inside are the data we display.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    21 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Network.h"

Inkplate display;

Network network;

#define DELAY_MS 300000 // Delay in milliseconds between deep sleep and the next wake up -> 5 minutes

char *ssid = "";     // your network SSID (name of wifi network)
char *password = ""; // your network password

int goal = 10000; // Set the goal manually to calculate how many are reached

int timeZone = 1; // Time zone for adding hours

int backers, pledged; // Variables to store fetched data

struct tm timeinfo; // Variable to store human-readable time

void setup()
{
    // Initialize serial and wait for port to open:
    Serial.begin(115200);
    delay(100);

    // Initialize Inkplate
    display.begin();

    // Connect inkplate to wifi
    network.begin(ssid, password);

    // Set the current time
    network.setTime(&timeinfo, timeZone);

    // Get data from the API
    Serial.println("Getting data from API");
    network.getData(&backers, &pledged);

    // Draw data on the display
    Serial.println("Drawing");
    drawData(backers, pledged, &timeinfo);

    // Go to sleep before checking again
    Serial.print("Going to deep sleep, bye.");
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

void drawData(int backers, int pledged, struct tm *timeinfo)
{
    // Print data in the frame buffer the Inkplate
    display.clearDisplay();
    display.setTextSize(2);
    display.drawTextWithShadow(0, 0, "Inkplate 2", INKPLATE2_RED, INKPLATE2_BLACK);

    display.setCursor(0, 25);
    display.print("Pledged: ");
    display.print(pledged);
    display.println("$");

    // Calculate the percentage of the reached value
    float reached = ((float)pledged / goal) * 100;

    display.print("Reached: ");
    display.print(reached, 2);
    display.println("%");

    display.print("Backers: ");
    display.println(backers);

    display.setCursor(0, 95);
    display.setTextSize(1);
    display.print("Updated: ");
    display.print(asctime(timeinfo));

    display.display(); // Display data to the Inkplate
}
