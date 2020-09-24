/*
   Inkplate_VariPass_Graphs example for e-radionica Inkplate6
   For this example you will need a micro USB cable, Inkplate6, and an available WiFi connection.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use the API on the VariPass website to download and display
   a sensor graph on the e-paper display.

   VariPass is a website which allows you to host various online "variables" which you can write to
   and read from using the VariPass API. This allows you to store sensor logs and later retrieve them
   for graphing, analysis, etc.
   This example uses an already public variable as an example. The graph is fed every minute with data
   from Thorinair's (https://github.com/Thorinair/) geiger counter, so each startup of the Inkplate will
   display updated values.

   To learn more about VariPass and how you can use it for your own projects, please visit: https://varipass.org/
   If you want to easily integrate the read/write functionality in your project, use the official library:
   https://github.com/Thorinair/VariPass-for-ESP8266-ESP32

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   23 July 2020 by e-radionica.com
*/

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "WiFi.h"                //Include library for WiFi
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char *ssid = "YourWiFiSSID"; // Your WiFi SSID
const char *password = "YourPass"; // Your WiFi password

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.print("Connecting to WiFi...");
    display.partialUpdate();

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        display.print(".");
        display.partialUpdate();
    }
    display.println("\nWiFi OK! Downloading...");
    display.partialUpdate();

    // Use a HTTP get request to fetch the graph from VariPass.
    // The API expects a few parameters in the URL to allow it to work.
    //  action - Should be set to "sgraph" or "graph" in order to generate a compatible image.
    //  id     - ID of the variable. It is enough to specify just the ID if the variable is public,
    //           but a "key" parameter should also be specified if not.
    //  width  - Width of the generated graph, here set to half the Inkplate's width.
    //  height - Height of the generated graph, here set to half the Inkplate's height.
    //  eink   - Should be set to true to generate a BW 1 bit bitmap better suitable for Inkplate.
    // For more detailed explanation and more parameters, please visit the docs page: https://varipass.org/docs/
    if (!display.drawBitmapFromWeb("https://api.varipass.org/?action=sgraph&id=kbg3eQfA&width=400&height=300&eink=true",
                                   200, 150))
    {
        display.println("Image open error");
        display.partialUpdate();
    }
    display.partialUpdate();

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}
