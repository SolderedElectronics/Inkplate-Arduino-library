/**
 **************************************************
 * @file        Inkplate13SPECTRA_Show_Pictures_From_Web.ino
 * @brief       Download and display an image from the web on Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Shows how you can download an image from the web and display that
 *              image on the e-paper display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable
 * - Extra:      Available WiFi connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials and the image URL in the sketch.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The image is downloaded and shown on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

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
