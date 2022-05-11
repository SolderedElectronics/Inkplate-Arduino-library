/*
    Youtube subscribers tracker example for Soldered Inkplate 2
    For this example you will need only USB cable and Inkplate 2.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 sends API call and server returns HTML document containing one
    quote and some information about it, then using library ArduinoJSON we extract only quote
    from JSON data and show it on Inkplate 2.

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/LoveLetter_Regular10.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 300

// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true); // Set text wrapping to true
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Our begin function
    network.begin();

    while (!network.getData(quote, author))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    // Full refresh
    display.display();

    // Go to sleep before checking again
    // This is set in microseconds, so it needs to be
    // multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    display.setFont(&LoveLetter_Regular10); // Set custom font
    display.setTextSize(1);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setCursor(0, 15);
    display.println(quote); // Print quote
    display.setCursor(30,100);
    display.print("-");
    display.println(author); // Print quote
}
