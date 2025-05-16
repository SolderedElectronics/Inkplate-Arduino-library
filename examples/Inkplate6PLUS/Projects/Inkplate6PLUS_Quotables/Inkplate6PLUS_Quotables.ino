/*
    Inkplate6PLUS_Quotables example for Soldered Inkplate 6Plus
    For this example you will need only USB cable and Inkplate 6PLUS.
    Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
    Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 6PLUS. After displaying quote
    ESP32 goes to sleep and wakes up every 300 seconds to show new quote(you can change time interval).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/FreeMonoBold24pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_1BIT);

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
    display.begin();
    display.setTextColor(BLACK);
    display.setTextWrap(false);
    display.clearDisplay();
    display.display();

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

    Serial.print("Retrying retriving data");
    while (!network.getData(quote, author))
    {
        Serial.print('.');
        delay(1000);
    }

    display.clearDisplay();
    //Draw the quote inside a textbox element
    display.drawTextBox(48, display.height() / 2 - 36, display.width()-48,display.height()/2+200,quote,1,&FreeMonoBold24pt7b,36,false,38);

    //Print the author in the bottom right corner
    uint16_t w, h;
    int16_t x, y;
    display.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    display.setCursor(display.width() - w - 50, display.height() - 30); // Set cursor to fit author name in lower right corner
    display.print("-");
    display.println(author); // Print author
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(DELAY_S);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}
