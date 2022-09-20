/*
    Inkplate_Web_Server example for Soldered Inkplate 2
    For this example you will need a micro USB cable, Inkplate 2 and a device with WiFi and Internet brower (PC, Laptop,
    Smartphone, ...). Select "Inkplate 2(ESP32)" from Tools -> Board menu. Don't have "Inkplate 2(ESP32)" option? Follow
    our tutorial and add it: https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can use Inkplate as a small and simple standlone Web Server.
    You need to connect to Inkplate with WiFi and open IP address shown on Inkplate display.
    After opening IP address, you will see text box where you can type some text and after that you press "Send to
    display". Text will apper on Inkplate display! This is just simple example what you can do with it and of course,
    you can create much more complex stuff.

    HINT: You can change WiFi name and password of your Inkplate WIFi Access point by changing ssid and pass in #define
    macros!

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    29 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "Inter6pt7b.h"
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
    display.setFont(&Inter6pt7b);                           // Change font to Inter

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
    printText();
}

void printInfo()
{                             // This function updates screen with new data (text)
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
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("User text");
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setCursor(0, 20);
    display.print(txt);
    Serial.print("User text: ");
    Serial.println(txt);
    display.display(); // Send everything to screen (refresh the screen)
}