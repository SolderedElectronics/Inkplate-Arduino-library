/**
 **************************************************
 * @file        Inkplate2_HTTP_Request.ino
 * @brief       Connect to WiFi, perform a basic HTTP GET request, and print the
 *              raw HTML response to the e-paper display.
 *
 * @details     This example demonstrates a simple HTTP workflow on Inkplate 2:
 *              scan for nearby WiFi networks, connect to a specified SSID, then
 *              fetch a web page over plain HTTP using HTTPClient.
 *
 *              The response body (HTML) is read from the HTTP stream into a
 *              dynamically allocated buffer (ps_malloc) and then printed
 *              directly to the e-paper display and the Serial Monitor. This is
 *              intentionally a "raw output" example and does not parse HTML or
 *              extract structured data.
 *
 *              The display uses the Inkplate 2 1-bit mode with tri-color
 *              palette (BLACK/WHITE/RED). Network scan results are shown with
 *              RSSI values highlighted in red, and the downloaded HTML is
 *              printed in red text.
 *
 *              This example uses plain HTTP (no TLS). Many modern sites redirect
 *              to HTTPS or block HTTP, so if the request fails, try an
 *              HTTP-capable endpoint or use an HTTPS example with proper
 *              certificate handling. The sketch allocates a large buffer
 *              (100000 bytes), so make sure allocation succeeds and be mindful of
 *              memory limits in more complex sketches. The response is read until
 *              stream.available() is exhausted, which is a simple approach and not
 *              robust for chunked/streaming responses or very large pages.
 *
 *              Expected output: WiFi scan results (SSID + open/encrypted marker
 *              + RSSI) followed by the downloaded HTML content on the display,
 *              and connection progress plus the same HTML payload in the Serial
 *              Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, pass) and change the
 *    http.begin() URL if needed (HTTP only).
 * 3) Upload the sketch to Inkplate 2.
 * 4) Optionally open Serial Monitor at 115200 baud.
 * 5) The device scans and lists up to 10 nearby WiFi networks.
 * 6) It connects to the configured SSID, then performs an HTTP GET request.
 * 7) The raw HTML response is printed to the display and Serial Monitor.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-30
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

const char ssid[] = ""; // Your WiFi SSID
const char pass[] = ""; // Your WiFi password

Inkplate display; // Create an object on Inkplate library

void setup()
{
    Serial.begin(115200);    // Initialize serial communication with PC
    display.begin();         // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();  // Clear frame buffer of display
    display.setCursor(0, 0); // Set print position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE); // Set text color to black and background color to white
    display.println("Scanning for WiFi networks...");       // Write text
    display.display();                                      // Send everything to display (refresh display)

    int n =
        WiFi.scanNetworks(); // Start searching WiFi networks and put the nubmer of found WiFi networks in variable n
    Serial.println("Scanning networks");
    display.setCursor(0, 0); // Set print position
    if (n == 0)
    {
        // If you did not find any network, show the message and stop the program.
        display.print("No WiFi networks found!");
        display.display();
        while (true)
            ;
    }
    else
    {
        if (n > 10)
            n = 10; // If you did find, print name (SSID), encryption and signal strength of first 10 networks
        for (int i = 0; i < n; i++)
        {
            display.setTextColor(INKPLATE2_BLACK,
                                 INKPLATE2_WHITE); // Set text color to black and background color to white
            display.print(WiFi.SSID(i));
            display.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
            display.print(' ');
            display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE); // Set text color to red and background color to white
            display.println(WiFi.RSSI(i), DEC);
        }
        display.display(); // Refresh screen
    }

    display.clearDisplay();          // Clear everything in frame buffer
    display.setCursor(0, 0);         // Set print cursor to new position
    display.print("Connecting to "); // Print the name of WiFi network
    display.print(ssid);
    WiFi.begin(ssid, pass); // Try to connect to WiFi network
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(
            1000); // While it is connecting to network, display dot every second, just to know that Inkplate is alive.
    }
    Serial.println("Connected!");

    char *webData;
    webData = (char *)ps_malloc(100000); // Allocate array for the stream of data
    if (webData != NULL)
    {
        HTTPClient http;
        int m = 0;
        if (http.begin("http://example.com/index.html"))
        {
            // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page
            // :))
            if (http.GET() == 200)
            {
                // If connection was successful, try to read content of the Web page and display it on screen
                while (http.getStream().available()) // If data available store data in data variable
                {
                    webData[m++] = http.getStream().read();
                }
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
                display.print(webData);
                Serial.print(webData);
                display.display();
            }
        }
        free(webData); // Free allocated memory
    }
}

void loop()
{
    // Nothing
}
