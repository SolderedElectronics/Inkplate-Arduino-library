/*
    Inkplate13SPECTRA_Quotables example for Soldered Inkplate 13SPECTRA
    For this example you will need only USB cable and Inkplate 13SPECTRA.
    Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
    Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

    https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML 
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 13SPECTRA. After displaying quote
    ESP32 goes to sleep and wakes up every 300 seconds to show new quote(you can change time interval).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
    21 January 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/FreeMonoBold12pt7b.h"

// Our networking functions, declared in Network.cpp
#include "NetworkUtils.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
NetworkUtils network;

// create display object
Inkplate inkplate;

// Delay between API calls in seconds, 300 seconds is 5 minutes
// Since the function this is used in expects time in microseconds,
// we have to multiply with 1000000
#define DELAY_S 300 * 1000000
#define DELAY_WIFI_RETRY_SECONDS 5
// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    inkplate.begin();
    inkplate.setTextColor(BLACK);
    inkplate.setTextWrap(true);
    inkplate.clearDisplay();
    inkplate.display();

    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!inkplate.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        //Can't connect to netowrk
        // Clear display for the error message
        inkplate.clearDisplay();
        // Set the font size;
        inkplate.setTextSize(3);
        // Set the cursor positions and print the text.
        inkplate.setCursor((inkplate.width() / 2) - 200, inkplate.height() / 2);
        inkplate.print(F("Unable to connect to "));
        inkplate.println(F(ssid));
        inkplate.setCursor((inkplate.width() / 2) - 200, (inkplate.height() / 2) + 30);
        inkplate.println(F("Please check SSID and PASS!"));
        // Display the error message on the Inkplate and go to deep sleep
        inkplate.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    Serial.println("Retrying retriving data");
    while (!network.getData(quote, author))
    {
        Serial.print('.');
        delay(1000);
    }

    inkplate.clearDisplay();
    //Draw the quote inside a textbox element
    inkplate.drawTextBox(48, inkplate.height() / 2 - 36, inkplate.width() - 48,inkplate.height() / 2 + 400, quote, 2, &FreeMonoBold12pt7b, 36, false, 24);

    //Print the author in the bottom right corner
    uint16_t w, h;
    int16_t x, y;
    inkplate.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    inkplate.setCursor(inkplate.width() - w - 50, inkplate.height() - 30); // Set cursor to fit author name in lower right corner
    inkplate.print("-");
    inkplate.println(author); // Print author
    inkplate.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(DELAY_S);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}