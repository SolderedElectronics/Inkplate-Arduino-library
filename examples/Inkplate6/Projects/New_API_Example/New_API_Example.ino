/*
   Cryptocurrency tracker example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display API data.
   Here we use Coingecko API to get last 90 days prices and display
   them on the Inkplate screen:
   https://www.coingecko.com/en/coins/bitcoin/historical_data/usd#panel
   If you wish to change the currecny, you can
   edit it below.

   IMPORTANT:
   Make sure to change your timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32_DEV
#error "Wrong board selection for this example, please select Inkplate 6 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "e-radionica.com";
char pass[] = "croduino";
char api_key[] = "";
char api_secret[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_3BIT);

// Delay between API calls in miliseconds (first 60 represents minutes so you can change to your need)
#define DELAY_MS 60 * 60 * 1000

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
    display.setTextColor(0, 7);

    if (refreshes == 0)
    {
        // Welcome screen
        display.setCursor(70, 230);
        display.setTextSize(2);
        display.println(F("Welcome to Inkplate 6 News API Example!"));
        display.setCursor(70, 250);
        display.println(F("Connecting to WiFi..."));
        display.display();
        display.clearDisplay();
        delay(1000);
    }

    // Our begin function
    network.begin();

    while (!network.getData(data))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // Full refresh
    display.display();

    ++refreshes;

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}
