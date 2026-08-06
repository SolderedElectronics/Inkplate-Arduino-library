/**
 **************************************************
 * @file        Inkplate5_HTTP_Web_Server.ino
 * @brief       Standalone WiFi web server example for Soldered Inkplate 5.
 *
 * @details     Demonstrates how to use Inkplate 5 as a simple standalone
 *              WiFi access point and HTTP web server. After connecting a PC,
 *              smartphone, or other WiFi-capable device to the Inkplate's
 *              access point, a web page can be opened in a browser where text
 *              can be entered and sent directly to the Inkplate e-paper display.
 *
 *              Inkplate operates as a WiFi Access Point (AP); the SSID and
 *              password can be changed in the code via #define macros. This is a
 *              basic demonstration intended for simple interaction and
 *              prototyping - more advanced web interfaces and logic can be built
 *              on top of it. Expected output is the Inkplate display showing its
 *              IP address and the received text, with the web page allowing
 *              custom text to be sent to the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      WiFi-capable device with a web browser (PC, laptop, smartphone)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5.
 * 3) Connect your device to the Inkplate WiFi access point.
 * 4) Open the IP address shown on the Inkplate display in a web browser.
 * 5) Enter text into the web page and press "Send to display".
 * 6) The submitted text appears on the Inkplate display.
 *
 * @note        Quick start guide: Inkplate 5 has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-02
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of our web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate 5"
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
    display.setCursor(140, 40); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.print("Connect to ");
    display.print(ssid);
    display.println(" WiFi with pass: ");
    display.setCursor(400, 100);
    display.println(pass);
    display.setCursor(230, 155);
    display.print("Open Your web browser and open");
    display.setCursor(330, 210);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    display.println();
    display.fillRect(10, 240, 940, 4, BLACK);
    display.println("User text:"); // Print out what user typed in web page
    display.print(txt);
    display.display(); // Send everything to screen (refresh the screen)
}
