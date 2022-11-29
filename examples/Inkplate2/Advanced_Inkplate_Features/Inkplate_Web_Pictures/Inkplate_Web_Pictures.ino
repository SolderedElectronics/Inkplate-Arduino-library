/*
    Web_BMP_pictures example for Soldered Inkplate 2
    For this example you will need a micro USB cable, Inkplate 2, and an available WiFi connection.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    You can open .bmp files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
    24 bit AND have resoluton smaller than 212x104 or otherwise it won't fit on screen.

    This example will show you how you can download a .bmp file (picture) from the web and
    display that image on e-paper display.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    28 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "HTTPClient.h" //Include library for HTTPClient
#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "WiFi.h"       //Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = "";     // Your WiFi SSID
const char password[] = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected!");

    // Draw the first image from web.
    // Monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares. Forth parameter will dither the image, but this image is already dithered
    // so it is not needed to dither it again while drawing.
    if (!display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                           "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced_Inkplate_Features/"
                           "Inkplate_Web_Pictures/dithered.jpg",
                           0, 0, false, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
    }
    display.display();

    delay(8000);

    // Draw the second image from web, this time using a HTTPClient to fetch the response manually.
    // Full color 24 bit images are large and take a long time to load, will take around 20 secs.
    HTTPClient http;
    // Set parameters to speed up the download process.
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    http.begin("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
               "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced_Inkplate_Features/"
               "Inkplate_Web_Pictures/Inkplate2.bmp");

    // Check response code.
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // Get the response length and make sure it is not 0.
        int32_t len = http.getSize();
        if (len > 0)
        {
            if (!display.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len))
            {
                // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
                // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
                // compression!
                display.println("Image open error");
            }
            display.display();
        }
        else
        {
            display.println("Invalid response length");
            display.display();
        }
    }
    else
    {
        display.println("HTTP error");
        display.display();
    }

    display.clearDisplay();
    delay(8000);

    // Try to load image and display it on e-paper at position X=0, Y=0
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. fourth parameter will dither the
    // image.
    if (!display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                           "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced_Inkplate_Features/"
                           "Inkplate_Web_Pictures/panzer.png",
                           0, 0, false, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.println("Image open error");
        display.display();
    }
    display.display();

    http.end();

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}
