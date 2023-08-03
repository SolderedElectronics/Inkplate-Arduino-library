/*
    Inkplate2_Image_Frame_From_Web example for Soldered Inkplate 2
    For this example you will need only a USB-C cable and Inkplate 2.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows how you can set Inkplate to show random pictures from web.
    What happens here is basically ESP32 connects to the WiFi and sends GET request
    to the random image on the server and the server returns a link to the image.
    Then Inkplate draws that image to the screen.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    23 May 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// Include Inkplate library in the sketch
#include "Inkplate.h"

// Create an object on Inkplate library
Inkplate display;

// ---------------- CHANGE HERE ---------------------:

// WiFi credentials
const char *ssid = ""; // Your WiFi SSID
const char *pass = ""; // Your WiFi password

// Define delay between 2 images in seconds
#define SECS_BETWEEN_IMAGES 30

// ---------------------------------------------------

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Join wifi
    display.connectWiFi(ssid, pass);
    Serial.println("Connected");

    // Get the URL of the next image
    char url[256];
    imageUrl(url);

    // Draw an image on the screen
    Serial.println(display.drawImage(url, display.PNG, 0, 0));
    display.display();

    // Go to deep sleep
    Serial.println("Going to sleep, bye");
    esp_sleep_enable_timer_wakeup(SECS_BETWEEN_IMAGES * 1000 * 1000LL); // Activate wakeup timer
    esp_deep_sleep_start(); // Start deep sleep (this function does not return). Program stops here.
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Get random image from web
void imageUrl(char *a)
{
    String url;
    HTTPClient http;

    // Do GET request
    if (http.begin("https://source.unsplash.com/random/212x104?crop=entropy&fit=crop&fm=png&h=104&w=212") && http.GET() > 0)
    {
        url = http.getString();

        int urlStart = url.indexOf("href=\"") + 6;
        int urlEnd = url.indexOf("\">", urlStart);

        url = url.substring(urlStart, urlEnd);
        url = url.substring(0, url.indexOf("?")) + "?crop=entropy&fit=crop&fm=png&h=104&w=212";

        // Print url to the Serial Monitor and copy to the buffer
        Serial.println(url);
        strcpy(a, url.c_str());
    }
    else
    {
        // Something went wrong, print an error message
        display.println("HTTP error");
        display.display();
    }
    http.end();
}
