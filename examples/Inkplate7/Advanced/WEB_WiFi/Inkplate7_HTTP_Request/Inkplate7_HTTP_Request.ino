/*
   Inkplate7_HTTP_Request example for Soldered Inkplate 7
   For this example you will need a USB-C cable, Inkplate 7 and stable WiFi Internet connection.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to connect to WiFi network, get data from Internet and display that data on ePaper.
   This example is NOT on to how to parse HTML data from Internet - it will just print HTML on the screen.
   You can see what's happening on the Serial monitor at the 115200 baud rate.

   In quotation marks you will need write your WiFi SSID and WiFi password in order to connect to your WiFi network.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   26 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

#include "HTTPClient.h" // Include HTTP library to this sketch
#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "WiFi.h"       // Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

Inkplate display; // Create an object on Inkplate library

void setup()
{
    Serial.begin(115200);                                 // Init serial communication
    display.begin();                                      // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();                               // Clear frame buffer of display
    display.setTextColor(INKPLATE7_BLACK, INKPLATE7_WHITE); // Set text color to black and background color to white
    Serial.println("Scanning for WiFi networks...");      // Write text

    // Start searching WiFi networks and put the nubmer of found WiFi networks in variable n
    int n = WiFi.scanNetworks(); 
    if (n == 0)
    {
        // If you did not find any network, show the message and stop the program.
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
            Serial.print(WiFi.SSID(i));
            Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
            Serial.print('\n');
            Serial.print(WiFi.RSSI(i), DEC);
        }
        Serial.println();
    }

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
    {
        // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page :))
        if (http.GET() > 0)
        {
            // If connection was successful, try to read content of the Web page and display it on screen
            String htmlText;
            htmlText = http.getString();
            display.print(htmlText);
            display.display();
        }
    }
}

void loop()
{
    // Nothing because we want to run the code only once
}
