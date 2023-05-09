/*
    Inkplate2_Show_Pictures_From_Web  example for Soldered Inkplate 2
    For this example you will need a micro USB cable, Inkplate 2, and an available WiFi connection.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how to simply download pictures from the web and
    display them on Inkplate 2's e-paper display.

    You can open .jpg, .png and.bmp files smaller than or exactly 212x104 px.

    .jpg files encoded with Baseline DCT, Huffman coding are supported
    .png and .bmp files are generally well supported
    The built-in dithering function works best on .png and .jpg

    If an image isn't displaying, open it in an image editor and save it as a different file.
    This usually resolves encoding issues.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    28 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "HTTPClient.h" // Include library for HTTPClient
#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "WiFi.h"       // Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = "";     // Your WiFi SSID
const char password[] = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);   // Init Serial communication.
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Set settings for error printing
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.setTextColor(BLACK);

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
    // Make sure the link is complete and correct (contains https:// or http://).
    // The example image is a monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares.
    // Fourth parameter will dither the image, but this image is already dithered
    // so it is not needed to dither it again while drawing.
    display.clearDisplay();

    if (!display.drawImage("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/cat_dithered.jpg",
                           0, 0, false, false))
    {
        // If is something failed (wrong url or unsupported format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    delay(8000);       // Wait a little bit

    // Draw the second image from web, this time using a HTTPClient to fetch the response manually.
    // Full color 24 bit images are large and take a long time to load, will take around 20 secs.
    HTTPClient http;
    // Set parameters to speed up the download process.
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    http.begin("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
               "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced/WEB_WiFi/"
               "Inkplate2_Show_Pictures_From_Web/car.bmp");

    // Check response code.
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // Get the response length and make sure it is not 0.
        int32_t len = http.getSize();
        if (len > 0)
        {
            if (!display.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len, true, false))
            {
                // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
                // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
                // compression!
                display.println("Image open error");
            }
            display.display(); // Refresh the display.
        }
        else
        {
            // If something goes wrong, print out the error message and refresh the display.
            display.println("Invalid response length");
            display.display();
        }
    }
    else
    {
        // Print out the error message and refresh the display.
        display.println("HTTP error");
        display.display();
    }

    display.clearDisplay(); // Clear the frame buffer
    delay(8000);            // Wait a little bit

    // Try to load image and display it on e-paper at position X=0, Y=0
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. fourth parameter will dither the
    // image.
    if (!display.drawImage("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "Inkplate2-DrawImage3Color-And-Examples/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/mountain.png",
                           0, 0, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.clearDisplay();
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    http.end();        // Close HTTP connection.

    WiFi.mode(WIFI_OFF); // Turn off the WiFi

    // Go to deep sleep
    Serial.println("Going to sleep..");
    display.setPanelDeepSleep(true); // Put the panel to deep sleep
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Nothing...
}
