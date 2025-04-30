/*
    Inkplate6_News_API Example for Soldered Inkplate 6
    This example demonstrates how to use the Inkplate 6 to display news headlines and descriptions
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
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
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
#include "FreeSerifItalic24pt7b.h"
#include "FreeSerifItalic12pt7b.h"

// Create network and display objects
Network network;
Inkplate display(INKPLATE_1BIT); // Use 1-bit grayscale mode for Inkplate6

// Constants for delays and refreshes
#define DELAY_MS (uint32_t)60 * 60 * 1000 // Delay between API calls (1 hour)
#define DELAY_WIFI_RETRY_SECONDS 10       // Delay for WiFi retry
#define DELAY_API_RETRY_SECONDS 10        // Delay for API retry
RTC_DATA_ATTR unsigned refreshes = 0;    // Counter for partial refreshes
const int fullRefresh = 20;              // Full refresh after 20 partial refreshes

// Function declarations
bool checkIfAPIKeyIsValid(char *APIKEY);
void setTime();
void drawNews(struct news *entities);

void setup()
{
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize the display
    display.begin();
    display.setTextWrap(false);

    // Connect to WiFi
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        // Display WiFi connection error
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor((display.width() / 2) - 200, display.height() / 2);
        display.print(F("Unable to connect to WiFi"));
        display.setCursor((display.width() / 2) - 200, (display.height() / 2) + 30);
        display.println(F("Check SSID and Password!"));
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // Validate the API key
    if (!checkIfAPIKeyIsValid(api_key_news))
    {
        // Display API key error
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor((display.width() / 2) - 200, display.height() / 2);
        display.print(F("Invalid API Key!"));
        display.setCursor((display.width() / 2) - 200, (display.height() / 2) + 30);
        display.println(F("Check your API key."));
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_API_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // Set the current time
    setTime();

    // Fetch news data and display it
    struct news *entities = network.getData();
    drawNews(entities);

    // Update the display
    display.display();

    // Increment refresh counter
    ++refreshes;

    // Enter deep sleep until the next update
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void drawNews(struct news *entities)
{
    display.setRotation(3); // Set display to landscape mode

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
    display.setFont(&Inter12pt7b);

    // Y position for the row
    int yPos = 83;

    // Print date left-aligned
    display.setCursor(35, yPos); // 23 px from left
    display.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 12; // adjust 12 for your font's avg char width
    int xRight = display.width() - updateStrWidth - 28; // 28 px margin from right

    // Print 'Last update' right-aligned
    display.setCursor(xRight, yPos);
    display.print(updateStr);

    for (int lineY = 93; lineY < 96; lineY++) // Adjust Y position for the line
    {
        display.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Render news items
    int startY = 140; // Adjust starting Y position for news items
    int boxHeight = 120; // Reduce box height for smaller screen
    int boxSpacing = 10; // Reduce spacing between boxes
    int leftMargin = 25; // Adjust margins
    int rightMargin = 170;
    int maxBoxes = (display.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title != NULL && entities[i].description != NULL; i++)
    {
        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        display.drawTextBox(leftMargin, y0, display.width() - rightMargin-20, y0 + 70,entities[i].title, 1, &GT_Pressura16pt7b, 26, false, 12);

        // Draw the description
        display.drawTextBox(leftMargin, y0 + 65, display.width() - rightMargin, y1,entities[i].description, 1, &FreeSerifItalic12pt7b, 20, false, 10);
    }
}


void setTime()
{
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
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));
}

bool checkIfAPIKeyIsValid(char *APIKEY)
{
    char *data = (char *)ps_malloc(50000LL);
    HTTPClient http;
    http.getStream().setTimeout(10);
    http.getStream().flush();
    http.getStream().setNoDelay(true);

    char *baseURL = "https://newsapi.org/v2/top-headlines?country=us&apiKey=";
    char apiTestURL[200];
    sprintf(apiTestURL, "%s%s", baseURL, APIKEY);

    http.begin(apiTestURL);
    delay(300);

    int httpCode = http.GET();
    bool failed = (httpCode != 200);

    if (failed)
    {
        Serial.println("Invalid API key.");
    }
    else
    {
        Serial.println("API key is valid.");
    }

    http.end();
    free(data);
    return !failed;
}

void loop()
{
    // Not used
}
