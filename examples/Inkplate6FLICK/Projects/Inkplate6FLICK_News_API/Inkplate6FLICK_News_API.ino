/*
    Inkplate6FLICK_News_API example for Soldered Inkplate 6FLICK
    For this example you will need only USB cable and Inkplate 6FLICK.
    Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
    Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 6FLICK to display API data.
    Here we use News API to get headline news and short description and display
    them on the Inkplate screen. For this you will need an API key which you can obtain
    here: https://newsapi.org/

    IMPORTANT:
    Make sure to change your timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    15 March 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";
char api_key_news[] = ""; //You can obtain one here: https://newsapi.org/

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "Inter12pt7b.h"
#include "GT_Pressura16pt7b.h"


// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_1BIT);

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

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void drawNews(struct news *entities)
{

    display.setRotation(3);
    uint8_t coll = 0;
    uint16_t y = 32;
    uint8_t rows = 0;
    int i = 0;
    while (coll < 2)
    {
        display.setCursor(10 + 379 * coll, y);
        display.setFont(&GT_Pressura16pt7b);
        uint16_t cnt = 0;
        while (*(entities[i].title + cnt) != '\0')
        {
            if (display.getCursorX() > 379 * coll + 359 || (*(entities[i].title + cnt) == ' ' && display.getCursorX() > 379 * coll + 314))
            {
                *(entities[i].title + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 32;
                display.setCursor(10 + 379 * coll, y);
            }
            
            if (display.getCursorY() > 984)
            {
                coll++;
                y = 32;
                display.setCursor(10 + 379 * coll, y);
            }
            display.print(*(entities[i].title + cnt));
            cnt++;
        }
        y = y + 40;
        display.setCursor(10 + 379 * coll, y);
        display.print("  ");
        
        cnt = 0;

        display.setFont(&Inter12pt7b);
        while (*(entities[i].description + cnt) != '\0')
        {
            if (display.getCursorX() > 379 * coll + 359 || (*(entities[i].description + cnt) == ' ' && display.getCursorX() > 379 * coll + 314))
            {
                *(entities[i].description + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 26;
                display.setCursor(10 + 379 * coll, y);
            }
            
            if (display.getCursorY() > 1004)
            {
                coll++;
                y = 32;
                display.setCursor(10 + 379 * coll, y);
            }
            display.print(*(entities[i].description + cnt));
            cnt++;
        }
        
        y += 48;
        i++;
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
    // Never here
}
