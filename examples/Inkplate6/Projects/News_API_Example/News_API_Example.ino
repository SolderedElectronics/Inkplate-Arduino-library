/*
    News API example for e-radionica.com Inkplate 6
    For this example you will need only USB cable and Inkplate 6.
    Select "Inkplate 6(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can use Inkplate 6 to display API data.
    Here we use Newsdata API to get headline news and short description and display
    them on the Inkplate screen. For this you will need an API key which you can obtain
    here: https://newsdata.io

    IMPORTANT:
    Make sure to change your timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    29 June 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32_DEV
#error "Wrong board selection for this example, please select Inkplate 6 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";
char api_key_news[] = ""; //You can obtain one here: https://newsdata.io

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "Inter10pt7b.h"
#include "GT_Pressura14pt7b.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_1BIT);

// Delay between API calls in miliseconds (first 60 represents minutes so you can change to your need)
#define DELAY_MS (uint32_t)60 * 60 * 1000

// Variable for counting partial refreshes
RTC_DATA_ATTR unsigned refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 20;

int nentities = 0; // Number of entities

void setup()
{
    // Begin serial communitcation with PC
    Serial.begin(115200);
    // Initial display settings
    display.begin();
    display.setTextWrap(false);

    if (refreshes == 0)
    {
        // Welcome screen
        display.setCursor(70, 230);
        display.setTextSize(1);
        display.println(F("Welcome to Inkplate 6 News API Example!"));
        display.setCursor(70, 250);
        display.println(F("Connecting to WiFi..."));
        display.display();
        display.clearDisplay();
        delay(1000);
    }

    // Our begin function
    network.begin();
    struct news *entities; // Create pointer to struct array where are news contained
    entities = network.getData(); // Create an API call and get data
    drawNews(entities); // Call function drawNews and show new on display
    display.display(); // Display all on display
    ++refreshes;

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void drawNews(struct news *entities)
{
    display.setRotation(3); // Set rotation of display to set it in portrait mode
    uint8_t coll = 0; // Counter for collumns
    uint16_t y = 28;  // Offset from top for first collumn
    uint8_t rows = 0; // Counter for printed rows of text
    int i = 0; // Counter for entities
    while (coll < 2) // Print two collumns
    {
        display.setCursor(10 + 300 * coll, y); // Set cursor accordingly to collumn and height of printed text
        display.setFont(&GT_Pressura14pt7b); // Set font for titile
        uint16_t cnt = 0; // Counter for printed characters
        while (*(entities[i].title + cnt) != '\0') // Print until stop character
        { // Check if characters are printed to end of line, if yes set new line
            if (display.getCursorX() > 300 * coll + 280 || (*(entities[i].title + cnt) == ' ' && display.getCursorX() > 300 * coll + 245))
            { // Check if characters are printed to end of line, if yes set new line
                *(entities[i].title + cnt) == ' ' ? cnt++ : 0; // Do not print space on begining of line
                rows++; // Increase row index
                y += 28; // Increase height for new line by 28 pixels
                display.setCursor(10 + 300 * coll, y); // Set cursor on beggining of next line
            }
            
            if (display.getCursorY() > 760) // If printed to bottom of page, create new collumn
            {
                coll++; // Increase collumn index
                y = 28; // Offset from top for next collumn
                display.setCursor(10 + 300 * coll, y); // Set cursor on beggining of next collumn
            }
            display.print(*(entities[i].title + cnt)); // Print one character
            cnt++; // Increase counter for characters
        }
        y = y + 38; // Offset from title to content
        display.setCursor(10 + 300 * coll, y); // Set cursor to print content
        display.print("  "); // Print few spaces or one tabulator
        
        cnt = 0; // Set characters counter to 0

        display.setFont(&Inter10pt7b); // Set font for content
        while (*(entities[i].content + cnt) != '\0') // Print until stop character
        { // Check if characters are printed to end of line, if yes set new line
            if (display.getCursorX() > 300 * coll + 280 || (*(entities[i].content + cnt) == ' ' && display.getCursorX() > 300 * coll + 255))
            { // Check if characters are printed to end of line, if yes set new line
                *(entities[i].content + cnt) == ' ' ? cnt++ : 0; // Do not print space on begining of line
                rows++; // Increase row index
                y += 22; // Increase height for new line by 22 pixels
                display.setCursor(10 + 300 * coll, y); // Set cursor on beggining of next line
            }
            
            if (display.getCursorY() > 780) // If printed to bottom of page, create new collumn
            {
                coll++; // Increase collumn index
                y = 28; // Offset from top for next collumn
                display.setCursor(10 + 300 * coll, y); // Set cursor on beggining of next collumn
            }
            display.print(*(entities[i].content + cnt)); // Print one character
            cnt++; // Increase counter for characters
        }
        y += 40; // Make offset for next new
        i++; // Increase index of news
        if(i > nentities) // If index of news is greater than number of new, break out of while loop
        {
          break;
        }
    }
}

void loop()
{
    // Never here
}
