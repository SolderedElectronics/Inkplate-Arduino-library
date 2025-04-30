/*
    Inkplate6COLOR_News_API Example for Soldered Inkplate 6COLOR
    This example demonstrates how to use the Inkplate 6COLOR to display news headlines and descriptions
    fetched from the News API. You will need an API key from https://newsapi.org/ to use this example.

    IMPORTANT:
    - Update your WiFi credentials and API key in the "CHANGE HERE" section below.
    - Ensure you have the ArduinoJSON library installed: https://arduinojson.org/
    - Adjust the timezone as needed.

    For more information, visit:
    - Inkplate documentation: https://www.inkplate.io
    - Support forums: https://forum.soldered.com/

    Created by Soldered, 30.4.2025
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// ---------- CHANGE HERE -------------
int timeZone = 2; // Adjust your timezone (e.g., 2 means UTC+2)

// WiFi credentials
char ssid[] = "YourWiFiSSID";
char pass[] = "YourWiFiPassword";

// News API key (get one from https://newsapi.org/)
char api_key_news[] = "YourNewsAPIKey";

// ------------------------------------

// Include necessary libraries
#include "Inkplate.h"
#include "Network.h"
#include "Inter12pt7b.h"
#include "GT_Pressura16pt7b.h"
#include "GT_Pressura8pt7b.h"
#include "FreeSerifItalic24pt7b.h"
#include "FreeSerifItalic12pt7b.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in miliseconds (first 60 represents minutes so you can change to your need)
#define DELAY_MS (uint32_t)60 * 60 * 1000
#define DELAY_WIFI_RETRY_SECONDS 5

// Variable for counting partial refreshes
RTC_DATA_ATTR unsigned refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 20;

void setTime();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);
    // Initial display settings
    display.begin();
    display.setTextWrap(false);

    // Connect Inkplate to the WiFi network
    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        //Can't connect to netowrk
        // Clear display for the error message
        display.clearDisplay();
        // Set the font size;
        display.setTextSize(3);
        // Set the cursor positions and print the text.
        display.setCursor((display.width() / 2) - 200, display.height() / 2);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.setCursor((display.width() / 2) - 200, (display.height() / 2) + 30);
        display.println(F("Please check SSID and PASS!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    setTime();

    struct news *entities;

    entities = network.getData();
    drawNews(entities);

    display.display();

    ++refreshes;

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    
    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void drawNews(struct news *entities)
{
    display.setRotation(3); // Set display to landscape mode
    display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE);
    // Display the title "World News"
    display.setFont(&FreeSerifItalic24pt7b);
    int textWidth = strlen("World News") * 10; // Adjust width calculation for smaller screen
    int centerX = (display.width() - textWidth) / 2;
    display.setCursor(centerX-60, 40); // Adjust Y position for smaller screen
    display.print("World News");

    // Draw a dividing line below the title
    int xStart = display.width() * 0.05; // Adjust margins for smaller screen
    int xEnd = display.width() * 0.95;
    for (int lineY = 60; lineY < 63; lineY++) // Adjust Y position for the line
    {
        display.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

        // Display the current date and time
    struct tm timeInfo;
    time_t nowSec;
    display.getNTPEpoch(&nowSec);
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        nowSec = time(nullptr);
    }
    gmtime_r(&nowSec, &timeInfo);

        // Prepare date and time strings
    char dateStr[20];
    char updateStr[20];
    sprintf(dateStr, "Date : %02d.%02d.%04d",
            timeInfo.tm_mday,
            timeInfo.tm_mon + 1,
            timeInfo.tm_year + 1900);

    sprintf(updateStr, "Last update : %02d.%02d",
            timeInfo.tm_hour,
            timeInfo.tm_min);

    // Choose font
    display.setFont(&GT_Pressura8pt7b);

    // Y position for the row
    int yPos = 83;

    // Print date left-aligned
    display.setCursor(25, yPos); // 23 px from left
    display.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 8; // adjust 8 for your font's avg char width
    int xRight = display.width() - updateStrWidth; // 28 px margin from right

    // Print 'Last update' right-aligned
    display.setCursor(xRight, yPos);
    display.print(updateStr);

    for (int lineY = 93; lineY < 96; lineY++) // Adjust Y position for the line
    {
        display.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Render news items
    int startY = 130; // Adjust starting Y position for news items
    int boxHeight = 90; // Reduce box height for smaller screen
    int boxSpacing = 10; // Reduce spacing between boxes
    int leftMargin = 25; // Adjust margins
    int rightMargin = 25;
    int maxBoxes = (display.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title != NULL && entities[i].description != NULL; i++)
    {
        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        display.drawTextBox(leftMargin, y0, display.width() - rightMargin-100, y0 + 60,entities[i].title, 1, &Inter12pt7b, 18, false, 12);

        // Draw the description
        display.drawTextBox(leftMargin, y0 + 60, display.width() - rightMargin+50, y1,entities[i].description, 1, &GT_Pressura8pt7b, 14, false, 10);
    }
}

// Function for getting time from NTP server
void setTime()
{
    // Structure used to hold time information
    struct tm timeInfo;
    time_t nowSec;
    // Fetch current time in epoch format and store it
    display.getNTPEpoch(&nowSec);
    // This loop ensures that the NTP time fetched is valid and beyond a certain threshold
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        nowSec = time(nullptr);
    }
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}