/**
 **************************************************
 * @file        Inkplate2_HTTP_Web_Server.ino
 * @brief       Simple standalone HTTP web server: create a WiFi access point,
 *              accept text from a browser, and render it on Inkplate 2.
 *
 * @details     This example turns Inkplate 2 into a small standalone web server
 *              by starting the ESP32 in WiFi Access Point (AP) mode and serving
 *              a minimal HTML page (stored in htmlCode.h). A client connects to
 *              the Inkplate-hosted WiFi network, opens the shown IP address in
 *              a browser, and submits a text string.
 *
 *              The submitted text is passed to the device via a URL route
 *              (/string/{...}), then displayed on the e-paper screen. The
 *              current AP SSID/password and the server IP address are also
 *              printed on the display to guide the user. The web server runs on
 *              port 80 (HTTP).
 *
 *              The display runs in 1-bit mode with the Inkplate 2 tri-color
 *              palette (BLACK/WHITE/RED). The sketch uses full refresh updates
 *              when showing instructions and when showing the submitted text.
 *
 *              The device runs as an AP (no router required); range and
 *              performance depend on the environment and client device. Text is
 *              passed in the URL path, so very long strings or special characters
 *              may require encoding and can exceed typical URL length limits. No
 *              authentication beyond the AP password is implemented, so treat
 *              this as a demo and avoid exposing it in untrusted environments.
 *
 *              Expected output: instructions (AP SSID/password + IP address) on
 *              the display, then "User text" and the submitted string
 *              (highlighted using the red colour); a simple page served from the
 *              Inkplate in the browser; and server start/IP plus received user
 *              text in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Phone/PC with WiFi + web browser
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Optionally edit #define ssid / #define pass to change the AP credentials.
 * 3) Upload the sketch to Inkplate 2.
 * 4) On your phone/PC, connect to the WiFi network shown on the display
 *    (default SSID "Inkplate", password "soldered").
 * 5) Open a browser and navigate to the displayed address (http://<AP IP>/).
 * 6) Enter text in the page and send it to the display.
 * 7) The Inkplate updates the screen to show the received text.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-29
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of the web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate"
#define pass "soldered"

Inkplate display;     // Create an object on Inkplate library
WebServer server(80); // Create Web server on port 80 (HTTP port number)

IPAddress serverIP;
String txt;

void setup()
{
    Serial.begin(115200);   // Initialize serial communication with PC
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE); // Set text color to black and background color to white
    display.setTextWrap(true);                              // If text does not fit on screen, send it to new line

    WiFi.begin();            // Init. WiFi library
    WiFi.mode(WIFI_AP);      // Set WiFi to Access Point mode
    WiFi.softAP(ssid, pass); // Set SSID (WiFi name) and password for Access Point

    serverIP = WiFi.softAPIP(); // Get the server IP address

    server.on("/", handleRoot); // If you open homepage, go to handle root function
    server.on(UriBraces("/string/{}"),
              handleString); // If you send some text to Inkplate, go to handleString function. Note that {} brackets at
    // the end of address. That means that web address has some arguments (our text!).
    server.begin(); // Start the web server
    Serial.println("Server started!");
    printInfo();
}

void loop()
{
    server.handleClient(); // You have to constantly read if there is any new client connected to web server
}

void updateHTML()
{
    // This function will send response to client and send HTML code of our web page
    server.send(200, "text/html", s);
}

void handleRoot()
{
    // This function will send response to client if client open a root (homepage) of our web page
    updateHTML();
}

void handleString()
{
    // This function will send response to client, send HTML code of web page, get the text from argument sent in web
    // page address and refresh screen with new text
    txt = server.arg(0);
    updateHTML();
    printText();
}

void printInfo()
{
    // This function updates screen with new data (text)
    display.clearDisplay();   // Clear everything from epaper frame buffer
    display.setTextSize(1);   // Scale text to original size (5x7 px)
    display.setCursor(5, 16); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.print("Connect to \"");
    display.print(ssid);
    display.print("\" WiFi");
    display.setCursor(5, 36);
    display.print("with pass: ");
    display.println(pass);
    display.setCursor(5, 56);
    display.print("Open Your web browser and open");
    display.setCursor(5, 76);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    Serial.print("IP address: ");
    Serial.println(serverIP);
    display.display();
}

void printText()
{
    display.setTextSize(1); // Scale text to an original size
    display.clearDisplay(); // Clear the frame buffer
    display.setCursor(0, 10);
    display.print("User text");
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setCursor(0, 20);
    display.print(txt); // Print out the user text
    Serial.print("User text: ");
    Serial.println(txt); // Also print user text on serial.
    display.display();   // Send everything to screen (refresh the screen)
}
