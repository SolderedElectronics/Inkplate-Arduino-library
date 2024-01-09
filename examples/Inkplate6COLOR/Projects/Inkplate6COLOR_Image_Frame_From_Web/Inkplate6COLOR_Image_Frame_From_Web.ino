/*
    Inkplate6COLOR_Image_Frame_From_Web example for Soldered Inkplate 6COLOR
    For this example you will need only USB cable and Inkplate 6COLOR.
    Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
    Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows how you can set inkplate to show random pictures from web.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    28 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display;

const char *ssid = "";     // Your WiFi SSID
const char *password = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);
    display.begin();

    // Join wifi
    display.connectWiFi(ssid, password);
    Serial.println("joined wifi");
    char url[256];
    imageUrl(url);

    Serial.println(display.drawImage(url, display.PNG, 0, 0, true, false));
    display.display();

    Serial.println("Going to sleep");

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(15ll * 60 * 1000 * 1000);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void imageUrl(char *a)
{
    String url;
    HTTPClient http;
    if (http.begin("https://source.unsplash.com/random/600x448") && http.GET() > 0)
    {
        url = http.getString();

        int urlStart = url.indexOf("href=\"") + 6;
        int urlEnd = url.indexOf("\">", urlStart);

        url = url.substring(urlStart, urlEnd);
        url = url.substring(0, url.indexOf("?")) + "?crop=entropy&fit=crop&fm=png&h=448&w=600";

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
