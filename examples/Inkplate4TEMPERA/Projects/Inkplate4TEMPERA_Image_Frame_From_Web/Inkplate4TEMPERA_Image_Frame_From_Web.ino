/*
   Inkplate4TEMPERA_Image_Frame_From_Web example for Soldered Inkplate 4TEMPERA
   For this example you will need only USB-C cable and Inkplate 4TEMPERA.
   Select "Soldered Inkplate 4TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   24 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4TEMPERA in the boards menu."
#endif

// Include Inkplate library in the sketch
#include "Inkplate.h"

// Create an object on Inkplate library
Inkplate display(INKPLATE_3BIT);

// ---------------- CHANGE HERE ---------------------:

// WiFi credentials
const char *ssid = "Soldered"; // Your WiFi SSID
const char *pass = "dasduino"; // Your WiFi password

// Define delay between 2 images in seconds
#define SECS_BETWEEN_IMAGES 30

// Here, enter the topic or topics of the kind of images you want to be displayed
// If you want to enter 2 topics, separate them with a comma without whitespace
char *topic = "animals,city";

// ---------------------------------------------------

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Join Wifi
    display.connectWiFi(ssid, pass);
    Serial.println("Connected");
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

    // Make an url
    char temp[100];
    sprintf(temp, "https://source.unsplash.com/random/600x600/?%s", topic);

    // Do GET request
    if (http.begin(temp) && http.GET() > 0)
    {
        url = http.getString();

        int urlStart = url.indexOf("href=\"") + 6;
        int urlEnd = url.indexOf("\">", urlStart);

        url = url.substring(urlStart, urlEnd);
        url = url.substring(0, url.indexOf("?")) + "?crop=entropy&fit=crop&fm=png&h=600&w=600";

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