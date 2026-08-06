/**
 **************************************************
 * @file        Inkplate6PLUS_Quotables.ino
 * @brief       Fetch a random quote from the Quotables API over WiFi, display it
 *              in 1-bit (BW) mode, then deep-sleep between updates.
 *
 * @details     This example connects Inkplate 6PLUS to WiFi and retrieves a
 *              random quote (plus author) from the public Quotables service.
 *              The API response is JSON; the sketch parses it (via helper code
 *              in QuotablesNetwork.*) and renders only the quote text and the
 *              author name on the e-paper display.
 *
 *              The quote is drawn inside a text box for wrapping, and the author
 *              is right-aligned near the bottom of the screen. The display runs
 *              in 1-bit (BW) mode (INKPLATE_1BIT) and uses a full refresh after
 *              drawing the content.
 *
 *              After displaying a quote, the ESP32 enters deep sleep for a
 *              configurable interval. Deep sleep restarts the ESP32 on wake, so
 *              setup() runs again and a new quote is fetched each cycle.
 *              Internet outages or service changes can prevent retrieval, and
 *              the sketch retries fetching until it succeeds. Text length varies,
 *              so very long quotes may wrap heavily or exceed the chosen text
 *              box/font settings.
 *
 *              Expected output: a wrapped quote in a large font with the author
 *              printed at the bottom-right (prefixed with "-"), connection status
 *              and retry dots on the Serial Monitor, and - if WiFi fails - an
 *              error message followed by a short sleep before retrying.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access (internet connection required)
 * - Library:    ArduinoJson (required by the networking helper,
 *               https://arduinojson.org/)
 * - Serial:     115200 baud (optional; used for debug output)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid/pass) in the sketch and adjust
 *    DELAY_S (default: 300 seconds) if needed.
 * 3) Install ArduinoJson if it is not already available in your environment.
 * 4) Upload the sketch to Inkplate 6PLUS.
 * 5) After boot, the device connects to WiFi, fetches a quote, displays it, and
 *    deep-sleeps; it wakes periodically and repeats.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2024-03-14
 * @license     GNU GPL V3
 **************************************************/

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
