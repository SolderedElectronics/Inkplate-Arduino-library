/**
 **************************************************
 * @file        Inkplate13SPECTRA_HTTP_Web_Server.ino
 * @brief       Standalone WiFi web server example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to use Inkplate as a small and simple standalone web
 *              server. Connect to the Inkplate with WiFi and open the IP address
 *              shown on the Inkplate display. After opening the IP address you
 *              will see a text box where you can type some text; press "Send to
 *              display" and the text appears on the Inkplate display. This is
 *              just a simple example of what you can do with it - of course, you
 *              can create much more complex things.
 *
 *              HINT: you can change the WiFi name and password of the Inkplate
 *              WiFi access point by changing ssid and pass in the #define macros.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable, device with WiFi and an
 *               Internet browser (PC, laptop, smartphone, ...)
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) Connect your device to the Inkplate WiFi access point.
 * 4) Open the IP address shown on the Inkplate display in a browser.
 * 5) Type text into the text box and press "Send to display".
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of our web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate 13SPECTRA"
#define pass "Soldered"

Inkplate inkplate;      // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
WebServer server(80);   // Create Web server on port 80 (HTTP port number)

IPAddress serverIP;
String txt;

void setup()
{
  inkplate.begin();                      // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.clearDisplay();               // Clear frame buffer of display
  inkplate.display();                    // Put clear image on display
  inkplate.setTextSize(4);               // Scale text to be four times bigger than original
  inkplate.setTextColor(INKPLATE_BLACK); // Set text color to black and background color to white
  inkplate.setTextWrap(true);            // If text does not fit on screen, send it to new line

  WiFi.begin();             // Init. WiFi library
  WiFi.mode(WIFI_AP);       // Set WiFi to Access Point mode
  WiFi.softAP(ssid, pass);  // Set SSID (WiFi name) and password for Access point

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
{ // This function updates screen with new data (text)
  inkplate.clearDisplay();    // Clear everything from epaper frame buffer
  inkplate.setCursor(20, 40); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
  inkplate.print("Connect to ");
  inkplate.print(ssid);
  inkplate.println(" WiFi with pass: ");
  inkplate.setCursor(240, 100);
  inkplate.println(pass);
  inkplate.setCursor(100, 150);
  inkplate.print("Open Your web browser and open");
  inkplate.setCursor(240, 210);
  inkplate.print("http://");
  inkplate.print(serverIP);
  inkplate.println('/');
  inkplate.println();
  inkplate.fillRect(10, 240, 780, 4, BLACK);
  inkplate.println("User text:"); // Print out what user typed in web page
  inkplate.print(txt);
  inkplate.display(); // Send everything to screen (refresh the screen)
}