/**
 **************************************************
 * @file        Inkplate6FLICK_HTTP_Web_Server.ino
 * @brief       Simple HTTP web server demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to run Inkplate 6FLICK as a standalone Wi-Fi
 *              Access Point with a simple HTTP web server. A client device
 *              (PC/phone/tablet) connects to the Inkplate Wi-Fi network and
 *              opens the displayed IP address in a browser. The served web
 *              page contains a text box; submitted text is sent back to the
 *              Inkplate and rendered on the e-paper display.
 *
 *              The example runs in Access Point mode, so no external router is
 *              required. Text wrapping is enabled, but very long text may still
 *              exceed the screen space. The HTML content is stored in htmlCode.h
 *              and served as a response. This is a basic demo that can be
 *              expanded with more endpoints/UI.
 *
 *              Expected output: the Inkplate shows the Wi-Fi SSID/password and
 *              the server IP address to open, the web page is served when the
 *              client opens the IP address, and the submitted text appears under
 *              "User text:" on the e-paper screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      PC / phone / tablet with Wi-Fi and a web browser
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Optionally change the Access Point SSID/password via the ssid/pass macros
 *    in the sketch.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) On the client device, connect to the Wi-Fi network "Inkplate 6FLICK"
 *    (or the SSID defined in the sketch) using the configured password.
 * 5) Open the shown IP address in a web browser (e.g. http://192.168.4.1/).
 * 6) Type text in the web page and press "Send to display".
 * 7) The submitted text appears on the Inkplate e-paper screen.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of our web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate 6FLICK"
#define pass "Soldered"

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
WebServer server(80);            // Create Web server on port 80 (HTTP port number)

IPAddress serverIP;
String txt;

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextSize(3);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
    display.setTextWrap(true);          // If text does not fit on screen, send it to new line

    WiFi.begin();            // Init. WiFi library
    WiFi.mode(WIFI_AP);      // Set WiFi to Access point mode
    WiFi.softAP(ssid, pass); // Set SSID (WiFi name) and password for Access point

    serverIP = WiFi.softAPIP(); // Get the server IP address

    server.on("/", handleRoot); // If you open homepage, go to handle root function
    server.on(UriBraces("/string/{}"),
              handleString); // If you send some text to Inkplate, go to handleString function. Note that {} brackets at
                             // the end of address. That means that web address has some arguments (our text!).
    server.begin();          // Start the web server
    updatePaper();
}

void loop()
{
    server.handleClient(); // You have to constantly read if there is any new client connected to web server
}

void updateHTML()
{ // This function will send response to client and send HTML code of our web page
    server.send(200, "text/html", s);
}

void handleRoot()
{ // This function will send response to client if client open a root (homepage) of our web page
    updateHTML();
}

void handleString()
{ // This function will send response to client, send HTML code of web page, get the text from argument sent in web page
  // address and refresh screen with new text
    txt = server.arg(0);
    updateHTML();
    updatePaper();
}

void updatePaper()
{                              // This function updates screen with new data (text)
    display.clearDisplay();    // Clear everything from epaper frame buffer
    display.setCursor(20, 40); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.print("Connect to ");
    display.print(ssid);
    display.println(" WiFi with pass: ");
    display.setCursor(240, 100);
    display.println(pass);
    display.setCursor(100, 150);
    display.print("Open Your web browser and open");
    display.setCursor(240, 210);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    display.println();
    display.fillRect(10, 240, 780, 4, BLACK);
    display.println("User text:"); // Print out what user typed in web page
    display.print(txt);
    display.display(); // Send everything to screen (refresh the screen)
}
