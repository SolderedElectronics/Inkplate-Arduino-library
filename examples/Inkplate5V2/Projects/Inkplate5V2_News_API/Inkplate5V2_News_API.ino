/*
    Inkplate5V2_News_API example for Soldered Inkplate 5 V2
    For this example you will need only a USB-C cable and Inkplate 5 V2
    Select "Soldered Inkplate5 V2" from Tools -> Board menu.
    Don't have "Soldered Inkplate5 V2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 5 V2 to display API data.
    Here we use News API to get headline news and short description and display
    them on the Inkplate screen. For this you will need an API key which you can obtain
    here: https://newsapi.org/
    On the Serial Monitor at 115200 baud rate, you can see what's happening.

    IMPORTANT:
    Make sure to change your timezone and wifi credentials below.
    Also have ArduinoJson installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    15 April 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your WiFi name (ssid) and password
char ssid[] = "";
char pass[] = "";
char apiKey[] = ""; // You can obtain one here: https://newsapi.org/

// Delay between API calls in miliseconds (first 60 represents minutes so you can change to your need)
// Here is set to 1 call per hour, but if you want to change it, have in mind that in the free plan there are only 100
// free API calls
#define DELAY_MS 60 * 60 * 1000
#define DELAY_WIFI_RETRY_SECONDS 5

//-------------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Include used fonts
#include "Fonts/GT_Pressura16pt7b.h"
#include "Fonts/Inter12pt7b.h"

// Create object with all networking functions
Network network;

// Create display object
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Init serial communication at 115200 baud rate for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
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

    // Pointer to the struct that will hold all news data
    struct news *entities = NULL;

    // Fetch news. If something went wrong the function returns NULL
    entities = network.getData(apiKey);
    if (entities == NULL)
    {
        Serial.println();
        Serial.println("Error fetching news");
        ESP.restart();
    }

    // Draw the news and display it on the screen
    drawNews(entities);
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000LL * DELAY_MS); // Set wakeup timer
    (void)esp_deep_sleep_start(); // Put ESP32 into deep sleep (this function returns nothing). Program stops here!
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}

// Function that draw the news
void drawNews(struct news *entities)
{
    uint8_t coll = 0; // For keeping track of columns
    uint16_t y = 32;  // Y coordinate for drawing
    uint8_t rows = 0; // For keeping track of rows
    int i = 0;        // For each piece of news

    // Printing the news until we fill 3 columns
    // If an entire piece of news doesn't fit on the screen, don't print it
    while (coll < 2)
    {
        display.setCursor(10 + 360 * coll, y); // Set the cursor to the beginning of the current column
        display.setFont(&GT_Pressura16pt7b);   // Set the font for the title
        uint16_t cnt = 0;                      // Index of each character in the title or description that is printing

        // Let's print the title
        while (*(entities[i].title + cnt) != '\0')
        {
            // Go to the new line if needed
            if (display.getCursorX() > 360 * coll + 280 ||
                (*(entities[i].title + cnt) == ' ' && display.getCursorX() > 360 * coll + 245))
            {
                *(entities[i].title + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 32;
                display.setCursor(10 + 360 * coll, y);
            }

            // Go to the next column if there is the end of the current one
            if (display.getCursorY() > E_INK_HEIGHT - 20)
            {
                coll++;
                y = 32;
                display.setCursor(10 + 360 * coll, y);
            }

            // Print the text in the frame buffer in before calculated positions
            display.print(*(entities[i].title + cnt));
            cnt++;
        }

        // Move the cursor a bit down and add indentation for the beginning of the sentence
        y = y + 40;
        display.setCursor(10 + 360 * coll, y);
        display.print("  ");

        // Reset the counter
        cnt = 0;

        // Set font for description and print description
        display.setFont(&Inter12pt7b);
        while (*(entities[i].description + cnt) != '\0')
        {
            // Go to the new line (row) if needed
            if (display.getCursorX() > 360 * coll + 280 ||
                (*(entities[i].description + cnt) == ' ' && display.getCursorX() > 360 * coll + 255))
            {
                *(entities[i].description + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 26;
                display.setCursor(10 + 360 * coll, y);
            }

            // Go to the next column if there is the end of the current one
            if (display.getCursorY() > E_INK_HEIGHT - 20)
            {
                coll++;
                y = 32;
                display.setCursor(10 + 360 * coll, y);
            }

            // Print the text in the frame buffer in before calculated positions
            display.print(*(entities[i].description + cnt));
            cnt++;
        }

        // Add a bit of spacing between 2 news and go to the other piece of news
        y += 48;
        i++;
    }
}
