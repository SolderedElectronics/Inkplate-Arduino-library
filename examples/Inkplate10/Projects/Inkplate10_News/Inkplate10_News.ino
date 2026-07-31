/**
 **************************************************
 * @file        Inkplate10_News.ino
 * @brief       Fetches latest news via NewsAPI.org over WiFi, renders headlines
 *              and descriptions in a newspaper layout, then deep-sleeps.
 *
 * @details     This example demonstrates a complete online-to-ePaper workflow
 *              on Inkplate 10. After boot, the ESP32 connects to WiFi, syncs
 *              time using NTP (required for timestamps and network services),
 *              then calls NewsAPI.org to retrieve a list of news items.
 *
 *              Parsed news entries (title + description) are laid out on the
 *              display using multiple fonts and text boxes: a centered "World
 *              News" header, a date/time row ("Date" and "Last update"), and a
 *              sequence of article blocks below. The display is driven in
 *              1-bit black/white mode (INKPLATE_1BIT) and updated with a full
 *              refresh; partial updates are not used.
 *
 *              After rendering, the ESP32 enters deep sleep for a fixed period
 *              (DELAY_MS). Deep sleep resets the ESP32, so the sketch restarts
 *              from setup() on every wake-up and fetches fresh headlines again.
 *
 *              Network reliability matters: API requests can fail due to
 *              connectivity, rate limits, or invalid credentials, and
 *              NewsAPI.org has usage limits depending on plan, so frequent
 *              polling may hit rate limits. Handle API key secrecy
 *              appropriately. TLS/HTTPS behavior depends on the networking
 *              implementation in src/Network.h; if certificates are not
 *              validated, treat it as demo-only and prefer proper certificate
 *              validation/pinning for production deployments.
 *
 *              Expected output: the e-paper display shows a "World News" page
 *              with current date / last update time and multiple news items
 *              (headline + description), while the Serial Monitor logs WiFi
 *              connection, time sync, fetch status and drawing progress for each
 *              news item.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable (battery optional for low-power testing)
 * - Extra:      WiFi Internet connection, NewsAPI.org API key
 * - Library:    ArduinoJson (install via Library Manager if not present)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Create an account at NewsAPI.org and generate an API key.
 * 3) Enter your WiFi SSID, password, API key and timeZone (UTC offset) in the
 *    "CHANGE HERE" section of the sketch.
 * 4) Upload the sketch and open Serial Monitor at 115200 baud.
 * 5) On boot, the device connects to WiFi, syncs time, fetches news, and draws
 *    the newspaper-style page.
 * 6) The board deep-sleeps for ~1 hour, then wakes and repeats the process.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-05-14
 * @license     GNU GPL V3
 **************************************************/

// Ensure the correct board is selected in the Arduino IDE
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
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
Inkplate inkplate(INKPLATE_1BIT);

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
    Serial.println(F("Starting Inkplate10_News example..."));

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
    inkplate.setRotation(3);

    // Display the title "World News"
    inkplate.setFont(&FreeSerifItalic24pt7b);
    int textWidth = strlen("World News") * 12; // Approximate width (12 pixels per character)
    int centerX = (inkplate.width() - textWidth) / 2;
    inkplate.setCursor(centerX - 70, 60); // Adjust text position to center
    inkplate.print("World News");

    // Draw a dividing line below the title
    int xStart = inkplate.width() * 0.03;
    int xEnd = inkplate.width() * 0.97;
    for (int lineY = 80; lineY < 83; lineY++)
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
    int yPos = 105;

    // Print date left-aligned
    inkplate.setCursor(23, yPos); // 23 px from left
    inkplate.print(dateStr);

    // Calculate width of 'Last update' string for right alignment
    int updateStrWidth = strlen(updateStr) * 12; // Adjust 12 for your font's avg char width
    int xRight = inkplate.width() - updateStrWidth - 19; // 19 px margin from right

    // Print 'Last update' right-aligned
    inkplate.setCursor(xRight, yPos);
    inkplate.print(updateStr);

    // Draw a line below the date
    for (int lineY = 113; lineY < 116; lineY++)
    {
        inkplate.drawLine(xStart, lineY, xEnd, lineY, BLACK);
    }

    // Render news items
    int startY = 160;
    int boxHeight = 170;
    int boxSpacing = 35;
    int leftMargin = 20;
    int rightMargin = 20;
    int maxBoxes = (inkplate.height() - startY) / (boxHeight + boxSpacing);

    for (int i = 0; i < maxBoxes && entities[i].title[0] != '\0' && entities[i].description[0] != '\0'; i++)
    {
        Serial.printf("Drawing news item %d...\n", i + 1);
        int y0 = startY + i * (boxHeight + boxSpacing);
        int y1 = y0 + boxHeight;

        // Draw the title
        inkplate.drawTextBox(leftMargin, y0 + 5, inkplate.width() - rightMargin, y0 + 80, entities[i].title, 1, &GT_Pressura16pt7b, 30, false, 18);

        // Draw the description
        inkplate.drawTextBox(leftMargin, y0 + 80, inkplate.width() - rightMargin, y1 + 20, entities[i].description, 1, &Inter12pt7b, 30, false, 16);
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
