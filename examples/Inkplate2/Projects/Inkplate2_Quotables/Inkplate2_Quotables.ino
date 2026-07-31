/**
 **************************************************
 * @file        Inkplate2_Quotables.ino
 * @brief       Fetch a random quote from a public API (no API key), render it
 *              on Inkplate 2, then deep sleep and refresh periodically.
 *
 * @details     This example demonstrates calling a simple public REST API that
 *              returns JSON (no authentication required), extracting the quote
 *              text and author using ArduinoJson via a NetworkFunctions helper,
 *              and displaying the result on Inkplate 2.
 *
 *              The quote is rendered inside a text box using drawTextBox() and
 *              the author is printed in the lower-right corner. drawTextBox()
 *              truncates with "..." if the quote exceeds the box height. After
 *              updating the display, the ESP32 enters deep sleep and wakes every
 *              DELAY_S microseconds (default: 5 minutes) to fetch and show a
 *              new quote.
 *
 *              If WiFi connection fails, an error message is shown and the
 *              device sleeps briefly before retrying. Because deep sleep resets
 *              the ESP32, execution always starts from setup() on each wake -
 *              keep logic there and leave loop() empty. Display mode is 1-bit
 *              (BW) with a full refresh (display()). API behavior/format may
 *              change over time; if parsing fails, update the NetworkFunctions
 *              implementation accordingly.
 *
 *              Expected output: the fetched quote centered in a text box with the
 *              author printed at the bottom-right prefixed with "-", fetch retry
 *              dots and basic status messages in the Serial Monitor, and on WiFi
 *              failure an "Unable to connect..." message followed by a short
 *              sleep and automatic retry.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Library:    ArduinoJson (required by the NetworkFunctions helper)
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Install ArduinoJson.
 * 3) Enter your WiFi SSID/password (ssid, pass), and optionally set DELAY_S
 *    (microseconds; 300 * 1,000,000 = 5 minutes) and
 *    DELAY_WIFI_RETRY_SECONDS.
 * 4) Upload the sketch to Inkplate 2.
 * 5) On each wake, the device fetches a quote and updates the display, then
 *    sleeps for the configured interval.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-04-07
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "NetworkFunctions.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
NetworkFunctions network;

// create display object
Inkplate display;

// Delay between API calls in seconds, 300 seconds is 5 minutes
// Since the function this is used in expects time in microseconds,
// we have to multiply with 1000000
#define DELAY_S 300 * 1000000
#define DELAY_WIFI_RETRY_SECONDS 5

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextColor(INKPLATE2_BLACK);
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
    display.drawTextBox(24, display.height() / 2 - 36, display.width()-24,display.height()/2+200,quote,1);

    //Print the author in the bottom right corner
    uint16_t w, h;
    int16_t x, y;
    display.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    display.setCursor(display.width() - w - 20, display.height() - 10); // Set cursor to fit author name in lower right corner
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