/*
   12-Inkplate_Web_JPEG_pictures for e-radionica Inkplate6
   For this example you will need a micro USB cable, Inkplate6, and an available WiFi connection.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   You can open .jpg files that have resoluton smaller than 800x600 or otherwise it won't fit on screen.

   This example will show you how you can download a .jpg file (picture) from the web and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   31 August 2020 by e-radionica.com
*/

#include "Inkplate.h"               //Include Inkplate library to the sketch
#include "HTTPClient.h"             //Include library for HTTPClient
#include "WiFi.h"                   //Include library for WiFi
Inkplate display(INKPLATE_1BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)

const char* ssid     = ""; //Your WiFi SSID
const char* password = ""; //Your WiFi password

void setup() {
    Serial.begin(115200);
    display.begin();        //Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); //Clear frame buffer of display
    display.display();      //Put clear image on display

    display.print("Connecting to WiFi...");
    display.partialUpdate();

    //Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        display.print(".");
        display.partialUpdate();
    }
    display.println("\nWiFi OK! Downloading...");
    display.partialUpdate();

    //Try to load image and display it on e-paper at position X=0, Y=100
    //NOTE: These methods require you to pass a reference to the display object as first parameter.
    //NOTE: Both drawJpegFromWeb methods allow for an optional sisxth "invert" parameter. Setting this parameter to true
    //will flip all colors on the image, making black white and white black.
    //fifth parameter will dither the image.
    if (!display.drawJpegFromWeb(&display, "https://varipass.org/destination.jpg", 0, 100, true, false)) {
        //If is something failed (wrong filename or format), write error message on the screen.
        display.println("Image open error");
        display.display();
    }
    display.display();

    WiFi.mode(WIFI_OFF);
}

void loop() {
    //Nothing...
}
