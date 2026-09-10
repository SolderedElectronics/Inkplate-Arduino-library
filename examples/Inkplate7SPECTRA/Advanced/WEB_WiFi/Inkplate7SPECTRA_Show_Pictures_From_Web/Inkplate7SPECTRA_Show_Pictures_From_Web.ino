/*
   Inkplate7SPECTRA_Show_Pictures_From_Web  example for Soldered Inkplate 7SPECTRA
   For this example you will need a micro USB cable, Inkplate 7SPECTRA, and an available WiFi connection.
   Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:

  /////////////////////////// INSERT BOARD DEF LINK HERE

   You can open .bmp files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
   24 bit AND have resoluton smaller than 800x480 or otherwise it won't fit on screen.

   This example will show you how you can download a .bmp file (picture) from the web and
   display that image on e-paper display.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

#include "HTTPClient.h" //Include library for HTTPClient
#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "WiFi.h"       //Include library for WiFi
Inkplate inkplate;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = ""; // Your WiFi SSID
const char *password = "";     // Your WiFi password

void setup()
{
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
    // Monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares. Forth parameter will dither the image. Photo taken by: Roberto Fernandez
    if (!inkplate.image.draw("https://varipass.org/neowise_mono.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        inkplate.println("Image open error");
        inkplate.display();
    }
    inkplate.display();

    if (!inkplate.image.draw("https://varipass.org/neowise.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        inkplate.println("Image open error");
        inkplate.display();
    }
    inkplate.display();

    inkplate.clearDisplay();
    delay(3000);

    // Try to load image and display it on e-paper at position X=0, Y=100
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. forth parameter will dither the
    // image.
    if (!inkplate.image.draw("https://varipass.org/destination.jpg", 0, 100, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
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