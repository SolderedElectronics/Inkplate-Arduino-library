/*
   InkplatePLUS2_HTTP_Web_Server example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2 and a device with WiFi and Internet brower (PC,
   Laptop, Smartphone, ...).

   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate as a small and simple standlone Web Server.
   You need to connect to Inkplate with WiFi and open IP address shown on Inkplate display.
   After opening IP address, you will se text box where you can type some text and after that you press "Send to
   display". Text will apper on Inkplate display! This is just simple example what you can do with it and of course, you
   can create much more complex stuff.

   HINT: You can change WiFi name and password of your Inkplate WIFi Access point by changing ssid and pass in #define
   macros!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   19 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "htmlCode.h"   //Include .h file where we stored out html code of our web page
#include <WebServer.h>  //Include ESP32 library for Web server
#include <WiFi.h>       //Include ESP32 WiFi library
#include <WiFiClient.h> //Include ESP32 WiFi library for AP
#include <uri/UriBraces.h>

#define ssid "Inkplate PLUS2"
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
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setCursor(30, 30);          // Set cursor position so text is visible
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
    display.setCursor(30, 30); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.print("Connect to: ");
    display.print(ssid);
    display.print(" WiFi");
    display.setCursor(30, 70);
    display.print("with password: ");
    display.print(pass);
    display.setCursor(30, 150);
    display.print("Open Your web browser and open");
    display.setCursor(30, 190);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    display.println();
    display.setCursor(30, 230);
    display.fillRect(10, 270, 580, 4, BLACK);
    display.println("User text:"); // Print out what user typed in web page
    display.setCursor(30, 290);
    display.print(txt);
    display.display(); // Send everything to screen (refresh the screen)
}
