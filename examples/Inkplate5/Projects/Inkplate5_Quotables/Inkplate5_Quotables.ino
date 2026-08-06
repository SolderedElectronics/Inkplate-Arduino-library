/**
 **************************************************
 * @file        Inkplate5_Quotables.ino
 * @brief       Fetches a random quote from the Quotables API over WiFi, displays
 *              it in 1-bit BW, then deep-sleeps between updates.
 *
 * @details     This example demonstrates making a simple HTTPS/HTTP API request
 *              to a public endpoint that does not require an API key, parsing a
 *              JSON response with ArduinoJson, and rendering the extracted text
 *              on Inkplate 5.
 *
 *              On boot, the ESP32 connects to WiFi using Inkplate's helper.
 *              It then repeatedly requests a new quote via QuotablesNetwork
 *              until valid data is received. The quote is drawn inside a text
 *              box using a large monospace font, and the author name is placed
 *              in the lower-right corner. The display operates in 1-bit
 *              black/white mode (INKPLATE_1BIT) and is updated with a full
 *              refresh.
 *
 *              After displaying the quote, the ESP32 enters deep sleep for a
 *              configurable interval (default 300 seconds). Deep sleep resets
 *              the ESP32, so the sketch restarts from setup() on every wake-up
 *              and no state is preserved. If WiFi connection fails, an error
 *              message is shown and the device sleeps briefly before retrying.
 *
 *              The networking layer extracts only the required fields (quote and
 *              author) from the JSON response; ArduinoJson capacity may need
 *              adjustment if the response format changes. DELAY_S is expressed
 *              in microseconds for esp_sleep_enable_timer_wakeup().
 *
 *              Expected output: a centered quote rendered in a large font with
 *              the author signature at the bottom-right (prefixed with a dash),
 *              Serial output showing retry progress while fetching data, and an
 *              on-screen error message followed by a short retry sleep if WiFi
 *              fails.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable (battery optional)
 * - Extra:      WiFi Internet connection
 * - Library:    ArduinoJson (Arduino Library Manager)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, pass) in the sketch.
 * 3) Upload the sketch and open Serial Monitor at 115200 baud (optional).
 * 4) After connecting, a random quote and author are fetched and displayed.
 * 5) The board deep-sleeps for ~5 minutes, then wakes to fetch a new quote.
 *
 * @note        Quick start guide: Inkplate 5 has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
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
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
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
#include "QuotablesNetwork.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
NetworkFunctions network;

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
