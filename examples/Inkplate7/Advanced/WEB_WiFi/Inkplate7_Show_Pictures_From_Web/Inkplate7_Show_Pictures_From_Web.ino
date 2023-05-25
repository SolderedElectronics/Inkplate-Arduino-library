/*
   Inkplate7_Show_Pictures_From_Web  example for Soldered Inkplate 7
   For this example you will need a micro USB cable, Inkplate 7, and an available WiFi connection.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   You can open .bmp files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
   24 bit AND have resoluton smaller than 640x384 or otherwise it won't fit on screen.

   This example will show you how you can download a .bmp file (picture) from the web and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   26 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

#include "HTTPClient.h" // Include library for HTTPClient
#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "WiFi.h"       // Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char *ssid = ""; // Your WiFi SSID
const char *pass = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);   // Init serial communication at 115200 baud rate
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Connect to the WiFi network.
    Serial.print("Connecting to WiFi...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);
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
    // bitmaps from certain softwares. Forth parameter will dither the image.
    if (!display.drawImage("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/dev/examples/Inkplate7/Advanced/WEB_WiFi/Inkplate7_Show_Pictures_From_Web/hearts_mono.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(3000);

    if (!display.drawImage("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/dev/examples/Inkplate7/Advanced/WEB_WiFi/Inkplate7_Show_Pictures_From_Web/roses.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
        display.display();
    }
    display.display();

    display.clearDisplay();
    delay(3000);

    // Try to load image and display it on e-paper at position X=0, Y=0
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. forth parameter will dither the
    // image.
    if (!display.drawImage("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/dev/examples/Inkplate7/Advanced/WEB_WiFi/Inkplate7_Show_Pictures_From_Web/flower.jpg", 0, 0, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.println("Image open error");
        display.display();
    }
    display.display();

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}
