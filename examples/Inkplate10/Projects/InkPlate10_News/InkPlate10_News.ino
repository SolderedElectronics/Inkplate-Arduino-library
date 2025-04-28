/*
InkPlate_News for Soldered Inkplate 10

This project fetches and displays news headlines using a news API.

Setup:
1. Enter your WiFi credentials.
2. Get an API key from https://newsapi.org/ or another news provider.
*/

#include "src/includes.h" // Include necessary libraries and dependencies

// --- WiFi Configuration ---
const char *ssid = "Soldered-testingPurposes";
const char *password = "Testing443";

// --- News API Configuration ---
String apiKey = "10fb57f24c784ee7be7a9cc419b775cc";
String newsEndpoint = "https://newsapi.org/v2/top-headlines?country=us&apiKey=" + apiKey;

// --- Device and Data Objects ---
Inkplate inkplate(INKPLATE_3BIT);
Network network;
Gui gui(inkplate);

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 600 // Sleep for 10 minutes

void setup()
{
    Serial.begin(115200);
    inkplate.begin();
    inkplate.clearDisplay();
    inkplate.setRotation(1);

    // Attempt to connect to WiFi
    const unsigned long timeout = 30000;
    unsigned long startTime = millis();
    bool connected = false;

    while (!connected && (millis() - startTime) < timeout)
    {
        connected = inkplate.connectWiFi(ssid, password, 10, true);
    }

    // If WiFi failed, display error message
    if (!connected)
    {
        gui.wifiError();
    }
    else
    {
        // Fetch and display news
        String newsData = network.fetchData(newsEndpoint);
        if (newsData.length() > 0)
        {
            gui.showNews(newsData); // Custom function to display news
        }
        else
        {
            gui.showError("Failed to fetch news.");
        }
    }

    // Enter deep sleep
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

void loop()
{
    // Empty, main logic is in setup
}