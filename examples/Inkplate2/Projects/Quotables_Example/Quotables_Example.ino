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

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true); // Set text wrapping to true
    display.setTextColor(BLACK, WHITE);

    // Our begin function
    network.begin();

    while (!network.getData(quote))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    // Full refresh
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS);
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
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 15);
    uint16_t cnt = 0;
    uint8_t len = strlen(quote);
    uint8_t rows = 0, colls = 0, row = 0;
    rows = len / 20;
    colls = ceil(len / (float)rows);
    display.setCursor(106 - colls * 10, 52 - rows * 20);
    while(*(quote + cnt) != '\0')
    {
        display.print(*(quote + cnt));
        cnt++;
        if(display.getCursorY() > 106 + colls * 10)
        {
          row++;
          display.setCursor(106 - colls * 10, 52 - rows * 20 + rows * 20);   
        }
    }
display.println(quote); // Print quote
}
