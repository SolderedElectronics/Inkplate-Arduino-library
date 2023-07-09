/*
   Inkplate4_Web_Server example for Soldered Inkplate 4
   For this example you will need a USB-C cable, Inkplate 4 and a device with WiFi and Internet browser (PC, Laptop,
   Smartphone, ...). Select "Soldered Inkplate 4" from Tools -> Board menu. 
   Don't have "Soldered Inkplate 4" option? Follow our tutorial and add it: 
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
   9 Jul 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

// Include needed libraries to the sketch
#include <Inkplate.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <uri/UriBraces.h>

// Include .h file where we stored out html code of our web page
#include "htmlCode.h"   

// Define name and password for Inkplate access point
#define ssid "Inkplate 4"
#define pass "Soldered"

Inkplate display;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
WebServer server(80); // Create Web server on port 80 (HTTP port number)

IPAddress serverIP;
String txt;

void setup()
{
    display.begin();                      // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();               // Clear frame buffer of display
    display.setTextSize(2);               // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(INKPLATE_BLACK); // Set text color to black and background color to white
    display.setTextWrap(true);            // If text does not fit on screen, send it to new line

    WiFi.begin();            // Init. WiFi library
    WiFi.mode(WIFI_AP);      // Set WiFi to Access point mode
    WiFi.softAP(ssid, pass); // Set SSID (WiFi name) and password for Access point

    serverIP = WiFi.softAPIP(); // Get the server IP address

    // If you open homepage, go to handle root function
    server.on("/", handleRoot); 

    // If you send some text to Inkplate, go to handleString function. Note that {} brackets at
    // the end of address. That means that web address has some arguments (our text!).
    server.on(UriBraces("/string/{}"), handleString); 

    // Start the web server and update the screen
    server.begin();          
    updatePaper();
}

void loop()
{
    // You have to constantly read if there is any new client connected to web server
    server.handleClient(); 
}

// This function will send response to client and send HTML code of our web page
void updateHTML()
{ 
    server.send(200, "text/html", s);
}

// This function will send response to client if client open a root (homepage) of our web page
void handleRoot()
{ 
    updateHTML();
}

// This function will send response to client, send HTML code of web page, get the text from argument sent in web page
// address and refresh screen with new text
void handleString()
{ 
    txt = server.arg(0);
    updateHTML();
    updatePaper();
}

// This function updates screen with new data (text)
void updatePaper()
{                              
    display.clearDisplay();    // Clear everything from epaper frame buffer
    display.setCursor(10, 20); // Print out instruction on how to connect to Inkplate WiFi and how to open a web page
    display.print("Connect to: ");
    display.print(ssid);
    display.println(" WiFi");
    display.setCursor(10, 50);
    display.print("with pass: ");
    display.println(pass);
    display.setCursor(10, 90);
    display.setTextColor(INKPLATE_RED);
    display.print("Open Your web browser and open:");
    display.setCursor(10, 115);
    display.print("http://");
    display.print(serverIP);
    display.println('/');
    display.println();
    display.fillRect(10, 145, 390, 4, INKPLATE_BLACK);
    display.setCursor(0, 160);
    display.setTextColor(INKPLATE_BLACK);
    display.println("User text:"); // Print out what user typed in web page
    display.print(txt);
    display.display(); // Send everything to screen (refresh the screen)
}
