/**
 **************************************************
 * @file        Inkplate6COLOR_News.ino
 * @brief       Connects to Wi-Fi, fetches news articles from News API, and
 *              displays headlines and descriptions on Inkplate 6COLOR.
 *
 * @details     This example demonstrates a connected content workflow on
 *              Inkplate 6COLOR using the News API service. The sketch connects
 *              to a Wi-Fi network, requests current news data from the API,
 *              parses the returned JSON payload, and renders article headlines
 *              and short descriptions on the e-paper display.
 *
 *              It is intended as a starting point for internet-connected
 *              dashboards, headline readers, and information panels built on
 *              Inkplate. Because the response is JSON-based, the example relies
 *              on ArduinoJson for parsing and formatting the incoming data
 *              before display; parsing consumes RAM, so response size should be
 *              considered carefully on embedded devices.
 *
 *              To use this example, you must provide valid Wi-Fi credentials,
 *              a News API key, and an appropriate timezone setting if the
 *              project formats times or dates locally. Wi-Fi reliability and API
 *              rate limits may affect refresh timing and request success. If
 *              HTTPS is used, certificate handling should be configured
 *              properly; insecure HTTPS modes such as setInsecure() are suitable
 *              for demos only and not recommended for production deployments.
 *              Display mode is Inkplate 6COLOR colour e-paper mode with full
 *              refreshes.
 *
 *              Expected output: news headlines and short article descriptions
 *              fetched from the configured News API request on the display, plus
 *              optional debug, connection or API status messages on the Serial
 *              Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi, News API key
 * - Library:    ArduinoJson
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Create an account at newsapi.org and generate an API key.
 * 3) Install the ArduinoJson library in Arduino IDE.
 * 4) Enter your Wi-Fi credentials, News API key and timezone settings in the
 *    sketch.
 * 5) Upload the sketch to Inkplate 6COLOR.
 * 6) After connecting to Wi-Fi, the board requests news data from the API.
 * 7) Parsed headlines and descriptions are rendered on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
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
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// ---------- CHANGE HERE -------------
// Adjust your timezone (e.g., 2 means UTC+2)
int timeZone = 2;

// WiFi credentials (replace with your WiFi network details)
char ssid[] = "";       // Replace with your WiFi SSID
char pass[] = "";   // Replace with your WiFi password

// News API key (get one from https://newsapi.org/)
char api_key_news[] = "0764e21041bc424c90071880e5f0607f"; // Replace with your News API key
// ------------------------------------


// Include necessary libraries
#include "Inkplate.h"
#include "src/Network.h"

// Include necessary libraries for fonts
#include "Fonts/Inter12pt7b.h"
#include "Fonts/GT_Pressura16pt7b.h"
#include "Fonts/FreeSerifBold12pt7b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"
#include "Fonts/Inter10pt7b.h"

// Create network and display objects
NetworkFunctions network;
Inkplate inkplate; // Use color mode for Inkplate 6COLOR

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
    Serial.println(F("Starting Inkplate6COLOR_News example..."));

    // Initialize the display
    inkplate.begin();
    inkplate.setTextWrap(false);
    Serial.println(F("Display initialized."));

    // Connect to WiFi
    Serial.println(F("Setting WiFi credentials..."));
    network.setCredentials(ssid, pass, api_key_news);
    network.setTimeZone(timeZone);
    Serial.println(F("Connecting to WiFi..."));
    network.begin();

    // Set the current time
    Serial.println(F("Setting time..."));
    setTime();

    // Fetch news data and display it
    Serial.println(F("Fetching news data..."));
    struct news *entities = network.getData(inkplate);
    if (entities != nullptr)
    {
        Serial.println(F("News data fetched successfully. Drawing news..."));
        drawNews(entities);
    }
    else
    {
        Serial.println(F("Failed to fetch news data."));
        // Display an error message if fetching news fails
        inkplate.clearDisplay();
        inkplate.setCursor(50, 230);
        inkplate.setTextSize(2);
        inkplate.println(F("Failed to fetch news"));
        inkplate.display();
    }

    // Update the display
    Serial.println(F("Updating display..."));
    inkplate.display();

    // Enter deep sleep until the next update
    Serial.println(F("Entering deep sleep..."));
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    esp_deep_sleep_start();
}

// Function to draw news items on the display
void drawNews(struct news *entities)
{
    inkplate.setRotation(3); // Set display to landscape mode
    inkplate.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE);
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
        inkplate.drawLine(xStart, lineY, xEnd, lineY, INKPLATE_BLACK);
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
    inkplate.setFont(&Inter10pt7b);

    // Y position for the row
    int yPos = 83;

    // Print date left-aligned
    inkplate.setCursor(20, yPos); // Adjusted for smaller screen
    inkplate.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 12; // Adjust 12 for your font's avg char width
    int xRight = inkplate.width() - updateStrWidth + 25; // Adjusted margin for smaller screen

    // Print 'Last update' right-aligned
    inkplate.setCursor(xRight, yPos);
    inkplate.print(updateStr);

    // Draw a line below the date
    for (int lineY = 93; lineY < 96; lineY++) // Adjust Y position for the line
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, INKPLATE_BLACK);
    }

    // Render news items
    int startY = 140; // Adjust starting Y position for news items
    int boxHeight = 120; // Reduce box height for smaller screen
    int boxSpacing = 10; // Reduce spacing between boxes
    int leftMargin = 10; // Adjust margins
    int rightMargin = 90;
    int maxBoxes = (inkplate.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title != nullptr && entities[i].description != nullptr; i++)
    {
        Serial.printf("Drawing news item %d...\n", i + 1);
        Serial.printf("Title: %s\n", entities[i].title);
        Serial.printf("Description: %s\n", entities[i].description);

        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        inkplate.drawTextBox(leftMargin, y0, inkplate.width() - rightMargin, y0 + 70, entities[i].title, 1, &FreeSerifBold12pt7b, 26, false, 12);

        // Draw the description
        inkplate.drawTextBox(leftMargin, y0 + 65, inkplate.width() - rightMargin-25, y1, entities[i].description, 1, &Inter10pt7b, 20, false, 10);
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