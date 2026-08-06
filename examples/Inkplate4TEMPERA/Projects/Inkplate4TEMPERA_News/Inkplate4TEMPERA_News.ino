/**
 **************************************************
 * @file        Inkplate4TEMPERA_News.ino
 * @brief       Fetch news headlines from NewsAPI.org over WiFi, render a
 *              "World News" layout, then deep sleep between updates.
 *
 * @details     This example connects Inkplate 4 TEMPERA to WiFi, synchronizes
 *              time via NTP (using the configured timezone), and fetches news
 *              data from NewsAPI.org using an API key. The response is parsed
 *              (via helper code in src/Network.h and ArduinoJson) into a list of
 *              news items, and the sketch renders a simple newspaper-style
 *              screen: title, current date/time, and multiple headline/summary
 *              boxes using custom fonts.
 *
 *              After updating the e-paper display, the ESP32 enters deep sleep
 *              for a fixed interval. When it wakes, the ESP32 restarts from
 *              setup(), fetches fresh news, and redraws the screen; loop() is not
 *              used after entering sleep.
 *
 *              Display mode is 1-bit (BW) and partial update is not used - the
 *              layout is drawn once and pushed with a full refresh (display()).
 *              NewsAPI.org enforces rate limits and plan restrictions, so if
 *              requests fail, check API limits, key validity and WiFi stability.
 *              The sketch waits until NTP time is valid before rendering
 *              date/time fields, so make sure timeZone matches your location.
 *              JSON parsing and multiple custom fonts can consume significant
 *              memory; if you experience instability, reduce the number of items
 *              fetched or simplify fonts/layout. HTTPS/TLS behavior depends on
 *              the implementation in src/Network.h - if the code uses insecure
 *              TLS settings (e.g. setInsecure()), treat it as demo-only and use
 *              proper certificate validation for production.
 *
 *              Expected output: a "World News" title, date and last update time,
 *              followed by a list of headline boxes with descriptions (as
 *              provided by NewsAPI.org), plus the current time printout and any
 *              network/debug output from the helper layer on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      WiFi (2.4 GHz), NewsAPI.org API key
 * - Library:    ArduinoJson (used by the network/parser layer)
 * - Serial:     115200 baud (recommended for debug/time output)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Install ArduinoJson in the Arduino IDE Library Manager.
 * 3) Create a NewsAPI.org account and generate an API key.
 * 4) Enter your WiFi SSID/password (ssid, pass), the API key (api_key_news) and
 *    timeZone (e.g. 2 for UTC+2) in the "CHANGE HERE" block.
 * 5) Upload the sketch.
 * 6) The device fetches news, renders the page once, then sleeps. It wakes and
 *    refreshes on the configured interval.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-30
 * @license     GNU GPL V3
 **************************************************/

// Ensure the correct board is selected in the Arduino IDE
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// ---------- CHANGE HERE -------------
// Adjust your timezone (e.g., 2 means UTC+2)
int timeZone = 2;

// WiFi credentials (replace with your WiFi network details)
char ssid[] = "YourWiFiSSID";       // Replace with your WiFi SSID
char pass[] = "YourWiFiPassword";   // Replace with your WiFi password

// News API key (get one from https://newsapi.org/)
char api_key_news[] = "YourNewsAPIKey"; // Replace with your News API key
// ------------------------------------

// Include necessary libraries
#include "Inkplate.h"
#include "src/Network.h"

// Include necessary libraries for fonts
#include "Fonts/Inter12pt7b.h"
#include "Fonts/GT_Pressura16pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"

// Create network and display objects
NetworkFunctions network;
Inkplate inkplate(INKPLATE_1BIT); // Use 1-bit grayscale mode for Inkplate 4TEMPERA

// Constants for delays and refreshes
#define DELAY_MS (uint32_t)60 * 60 * 1000 // Delay between API calls (1 hour)
#define DELAY_WIFI_RETRY_SECONDS 10       // Delay for WiFi retry
#define DELAY_API_RETRY_SECONDS 10        // Delay for API retry

// Function declarations
void setTime();
void drawNews(struct news *entities);

void setup()
{
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize the display
    inkplate.begin();
    inkplate.setTextWrap(false);

    // Connect to WiFi
    network.setCredentials(ssid, pass, api_key_news);
    network.setTimeZone(timeZone);
    network.begin();

    // Set the current time
    setTime();

    // Fetch news data and display it
    struct news *entities = network.getData(inkplate);
    if (entities != nullptr)
    {
        drawNews(entities);
    }
    else
    {
        // Display an error message if fetching news fails
        inkplate.clearDisplay();
        inkplate.setCursor(50, 230);
        inkplate.setTextSize(2);
        inkplate.println(F("Failed to fetch news"));
        inkplate.display();
    }

    // Update the display
    inkplate.display();

    // Enter deep sleep until the next update
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    esp_deep_sleep_start();
}

// Function to draw news items on the display
void drawNews(struct news *entities)
{
    inkplate.setRotation(0); // Set display to portrait mode
    
    // Display the title "World News"
    inkplate.setFont(&FreeSerifItalic24pt7b);
    int textWidth = strlen("World News") * 10; // Adjust width calculation for smaller screen
    int centerX = (inkplate.width() - textWidth) / 2;
    inkplate.setCursor(centerX - 60, 40); // Adjust Y position for smaller screen
    inkplate.print("World News");

    // Draw a dividing line below the title
    int xStart = inkplate.width() * 0.05; // Adjust margins for smaller screen
    int xEnd = inkplate.width() * 0.95;
    for (int lineY = 60; lineY < 63; lineY++) // Adjust Y position for the line
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Display the current date and time
    struct tm timeInfo;
    time_t nowSec;
    inkplate.getNTPEpoch(&nowSec);
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

    sprintf(updateStr, "Last update : %02d:%02d",
            timeInfo.tm_hour,
            timeInfo.tm_min);

    // Choose font
    inkplate.setFont(&Inter12pt7b);

    // Y position for the row
    int yPos = 83;

    // Print date left-aligned
    inkplate.setCursor(35, yPos); // Adjusted for smaller screen
    inkplate.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 12; // Adjust 12 for your font's avg char width
    int xRight = inkplate.width() - updateStrWidth - 28; // Adjusted margin for smaller screen

    // Print 'Last update' right-aligned
    inkplate.setCursor(xRight, yPos);
    inkplate.print(updateStr);

    // Draw a line below the date
    for (int lineY = 93; lineY < 96; lineY++) // Adjust Y position for the line
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Render news items
    int startY = 140; // Adjust starting Y position for news items
    int boxHeight = 120; // Reduce box height for smaller screen
    int boxSpacing = 10; // Reduce spacing between boxes
    int leftMargin = 25; // Adjust margins
    int rightMargin = 190;
    int maxBoxes = (inkplate.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title != nullptr && entities[i].description != nullptr; i++)
    {
        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        inkplate.drawTextBox(leftMargin, y0, inkplate.width() - rightMargin, y0 + 70, entities[i].title, 1, &GT_Pressura16pt7b, 26, false, 12);

        // Draw the description
        inkplate.drawTextBox(leftMargin, y0 + 65, inkplate.width() - rightMargin-50, y1, entities[i].description, 1, &Inter12pt7b, 20, false, 10);
    }
}

void setTime()
{
    struct tm timeInfo;
    time_t nowSec;
    inkplate.getNTPEpoch(&nowSec);
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
    // Not used
}