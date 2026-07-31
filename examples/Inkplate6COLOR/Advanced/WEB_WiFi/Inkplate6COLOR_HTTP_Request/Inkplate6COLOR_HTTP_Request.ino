/**
 **************************************************
 * @file        Inkplate6COLOR_HTTP_Request.ino
 * @brief       Connects Inkplate 6COLOR to Wi-Fi, performs an HTTP GET request,
 *              and displays the received HTML response.
 *
 * @details     This example demonstrates a complete basic Wi-Fi + HTTP workflow
 *              on Inkplate 6COLOR. The sketch first scans for nearby wireless
 *              networks and shows a short list of detected SSIDs with signal
 *              information. It then connects to the configured Wi-Fi network,
 *              performs a plain HTTP GET request to a web page, retrieves the
 *              server response as text, and prints that response directly on
 *              the e-paper display.
 *
 *              This example is intended to demonstrate network connectivity and
 *              basic web requests only. It does not parse HTML, extract data,
 *              or render web pages visually. The raw response body is simply
 *              displayed as text, which is useful for testing connectivity or
 *              understanding how to fetch remote content before adding parsing
 *              logic.
 *
 *              The example uses plain HTTP, not HTTPS, so traffic is not
 *              encrypted. Large responses may exceed practical display space or
 *              memory limits, so shorter text pages are more suitable for this
 *              workflow. For production web integrations, prefer fetching
 *              structured data such as JSON rather than printing full HTML
 *              pages. Display mode is Inkplate 6COLOR colour e-paper mode, using
 *              full refreshes.
 *
 *              Expected output: nearby Wi-Fi SSIDs during the scan, then raw
 *              HTML/text content fetched from the target web page on the
 *              display, with scan and Wi-Fi connection progress messages on
 *              Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Wi-Fi connection
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Enter your Wi-Fi SSID and password in the sketch, and update the target URL
 *    in http.begin() if you want to fetch a different page.
 * 3) Upload the example to Inkplate 6COLOR.
 * 4) Open Serial Monitor at 115200 baud to follow connection progress.
 * 5) On startup, the sketch scans nearby Wi-Fi networks and shows a short list
 *    of detected networks on the display.
 * 6) The board then connects to the configured Wi-Fi network.
 * 7) After connecting, the sketch performs an HTTP GET request and displays the
 *    returned HTML/text response on the screen.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

Inkplate display; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

void setup()
{
    Serial.begin(115200);                            // Begin Serial for debugging
    display.begin();                                 // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();                          // Clear frame buffer of display
    display.display();                               // Put clear image on display
    display.setTextSize(2);                          // Set text scaling to two (text will be two times bigger)
    display.setCursor(0, 0);                         // Set print position
    display.setTextColor(INKPLATE_BLACK);            // Set text color to black and background color to white
    Serial.println("Scanning for WiFi networks..."); // Write text

    int n =
        WiFi.scanNetworks(); // Start searching WiFi networks and put the nubmer of found WiFi networks in variable n
    if (n == 0)
    { // If you did not find any network, show the message and stop the program.
        Serial.print("No WiFi networks found!");
        while (true)
            ;
    }
    else
    {
        if (n > 10)
            n = 10; // If you did find, print name (SSID), encryption and signal strength of first 10 networks
        for (int i = 0; i < n; i++)
        {
            display.print(WiFi.SSID(i));
            display.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
            display.print('\n');
            display.print(WiFi.RSSI(i), DEC);
        }
        display.display();
    }

    display.clearDisplay();         // Clear everything in frame buffer
    display.setCursor(0, 0);        // Set print cursor to new position
    Serial.print("Connecting to "); // Print the name of WiFi network
    Serial.print(ssid);
    WiFi.begin(ssid, pass); // Try to connect to WiFi network
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000); // While it is connecting to network, display dot every second, just to know that Inkplate is
                     // alive.
        Serial.print('.');
    }
    Serial.print("connected"); // If it's connected, notify user

    HTTPClient http;
    if (http.begin("http://example.com/index.html"))
    { // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page :))
        if (http.GET() > 0)
        { // If connection was successful, try to read content of the Web page and display it on screen
            String htmlText;
            htmlText = http.getString();
            display.setTextSize(1); // Set smaller text size, so everything can fit on screen
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print(htmlText);
            display.display();
        }
    }
}

void loop()
{
    // Nothing
}
