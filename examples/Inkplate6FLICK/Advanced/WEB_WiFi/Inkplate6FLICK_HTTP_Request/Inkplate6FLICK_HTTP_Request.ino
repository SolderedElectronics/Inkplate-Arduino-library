/**
 **************************************************
 * @file        Inkplate6FLICK_HTTP_Request.ino
 * @brief       Basic HTTP GET request demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to connect Inkplate 6FLICK to a Wi-Fi network,
 *              perform a simple HTTP GET request, and display the received
 *              response on the e-paper display. This example prints the raw
 *              HTML/text response (it does not parse or extract data from it).
 *
 *              The example uses plain HTTP (not HTTPS). The response is printed
 *              directly, so long pages may not fit the screen. Wi-Fi scanning
 *              and connection status are displayed using partial updates.
 *              Expected output is a list of detected Wi-Fi networks (SSID,
 *              encryption marker, RSSI), the connection status while joining the
 *              configured network, and the raw HTML/text content from the
 *              requested web page shown on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      Wi-Fi connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Enter your Wi-Fi SSID and password in ssid/pass.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) The device scans for nearby Wi-Fi networks and lists up to 10 of them.
 * 5) Inkplate connects to the configured Wi-Fi network.
 * 6) A GET request is sent to a test URL and the response is printed on screen.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

void setup()
{
    display.begin();                                  // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();                           // Clear frame buffer of display
    display.display();                                // Put clear image on display
    display.setTextSize(2);                           // Set text scaling to two (text will be two times bigger)
    display.setCursor(0, 0);                          // Set print position
    display.setTextColor(BLACK, WHITE);               // Set text color to black and background color to white
    display.println("Scanning for WiFi networks..."); // Write text
    display.display();                                // Send everything to display (refresh display)

    int n = WiFi.scanNetworks(); // Start searching WiFi networks and put the nubmer of found WiFi networks in variable
                                 // n
    if (n == 0)
    { // If you did not find any network, show the message and stop the program.
        display.print("No WiFi networks found!");
        display.partialUpdate();
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
        display.partialUpdate(); //(Partial) refresh thescreen
    }

    display.clearDisplay();          // Clear everything in frame buffer
    display.setCursor(0, 0);         // Set print cursor to new position
    display.print("Connecting to "); // Print the name of WiFi network
    display.print(ssid);
    WiFi.begin(ssid, pass); // Try to connect to WiFi network
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000); // While it is connecting to network, display dot every second, just to know that Inkplate is
                     // alive.
        display.print('.');
        display.partialUpdate();
    }
    display.print("connected"); // If it's connected, notify user
    display.partialUpdate();

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
