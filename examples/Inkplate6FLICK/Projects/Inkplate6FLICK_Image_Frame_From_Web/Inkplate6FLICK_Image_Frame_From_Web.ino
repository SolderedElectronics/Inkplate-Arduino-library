/*
   Inkplate6FLICK_Image_Frame_From_Web example for Soldered Inkplate 6FLICK
   For this example you will need only USB cable and Inkplate 6FLICK.
   Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 March 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"

const char ssid[] = "";    // Your WiFi SSID
const char *password = ""; // Your WiFi password

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();

    // To turn off frontlight
    // display.frontlight(true);
    // display.setFrontlight(0);

    // If you want to use frontlight:
    // display.frontlight(true);
    // display.setFrontlight(35);

    // Join wifi
    display.connectWiFi(ssid, password);

    char url[256];
    imageUrl(url);

    Serial.println(display.drawImage(url, display.PNG, 0, 0));
    display.display();

    Serial.println("Going to sleep");
    delay(100);
    esp_sleep_enable_timer_wakeup(15ll * 60 * 1000 * 1000);
    esp_deep_sleep_start();
}

void loop()
{
    // Never here, as deepsleep restarts esp32
}

void imageUrl(char *a)
{
    String url;
    HTTPClient http;
    if (http.begin("https://source.unsplash.com/random/1024x768") && http.GET() > 0)
    {
        url = http.getString();

        int urlStart = url.indexOf("href=\"") + 6;
        int urlEnd = url.indexOf("\">", urlStart);

        url = url.substring(urlStart, urlEnd);
        url = url.substring(0, url.indexOf("?")) + "?crop=entropy&fit=crop&fm=png&h=768&w=1024";

        Serial.println(url);
        strcpy(a, url.c_str());
    }
    else
    {
        display.println("HTTP error");
        display.display();
    }
    http.end();
}
