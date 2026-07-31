/*
   Inkplate13SPECTRA_Show_Pictures_From_Web  example for Soldered Inkplate 13SPECTRA
   For this example you will need a micro USB cable, Inkplate 13SPECTRA, and an available WiFi connection.
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

  https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

   This example will show you how you can download an image from the web and
   display that image on e-paper display.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "HTTPClient.h" //Include library for HTTPClient
#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "WiFi.h"       //Include library for WiFi
Inkplate inkplate;       // Create an object on Inkplate library

const char ssid[] = "";     // Your WiFi SSID
const char *password = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);    // Init serial communication so we can see debug messages
    inkplate.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    inkplate.clearDisplay(); // Clear frame buffer of display
    inkplate.display();      // Put clear image on display

    Serial.print("Connecting to WiFi...");

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi OK! Downloading...");

    // Draw the first image from web.
    // This image is 1600x1200px, matching Inkplate 13SPECTRA's native resolution exactly,
    // so drawing it at (0, 0) fills the entire screen.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares. Forth parameter will dither the image.
    if (!inkplate.image.draw("https://i.imgur.com/ESkX8xU.jpeg", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        inkplate.println("Image open error");
        inkplate.display();
    }
    inkplate.display();
    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}
