/*
    Inkplate6PLUS_News_API example for Soldered Inkplate PLUS2
    For this example you will need only USB-C cable and Inkplate PLUS2.
    Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
    Don't have Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 6PLUS to display API data.
    Here we use News API to get headline news and short description and display
    them on the Inkplate screen. For this you will need an API key which you can obtain
    here: https://newsapi.org/

    IMPORTANT:
    Make sure to change your timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    27 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";
char api_key_news[] = ""; // You can obtain one here: https://newsapi.org/

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "GT_Pressura16pt7b.h"
#include "Inter12pt7b.h"
#include "Network.h"


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

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);
    // Initial display settings
    display.begin();
    display.setTextWrap(false);

    // Our begin function
    network.begin();

    struct news *entities;

    entities = network.getData();
    drawNews(entities);

    display.display();

    ++refreshes;

    Serial.println("Done!");

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void drawNews(struct news *entities)
{

    uint8_t coll = 0;
    uint16_t y = 32;
    uint8_t rows = 0;
    int i = 0;
    while (coll < 1)
    {
        display.setCursor(30, y);
        display.setFont(&GT_Pressura16pt7b);
        uint16_t cnt = 0;
        while (*(entities[i].title + cnt) != '\0')
        {
            if (display.getCursorX() > 550  || (*(entities[i].title + cnt) == ' ' && display.getCursorX() > 550))
            {
                *(entities[i].title + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 32;
                display.setCursor(30, y);
            }
            
            if (display.getCursorY() > 580)
            {
                coll++;
                y = 32;
                display.setCursor(650, y);
                break;
            }
            display.print(*(entities[i].title + cnt));
            cnt++;
        }
        y = y + 40;
        display.setCursor(30, y);
        display.print("  ");
        
        cnt = 0;

        display.setFont(&Inter12pt7b);
        while (*(entities[i].description + cnt) != '\0')
        {
            if (display.getCursorX() > 550  || (*(entities[i].description + cnt) == ' ' && display.getCursorX() > 550))
            {
                *(entities[i].description + cnt) == ' ' ? cnt++ : 0;
                rows++;
                y += 26;
                display.setCursor(30, y);
            }
            
            if (display.getCursorY() > 580)
            {
                coll++;
                y = 32;
                display.setCursor(650, y);
                break;
            }
            display.print(*(entities[i].description + cnt));
            cnt++;
        }
        
        y += 48;
        i++;
    }

}

void loop()
{
    // Never here
}
